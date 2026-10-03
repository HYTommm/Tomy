#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef OWNERSHIP_TEST

/* ============================================================================
   所有权回归测试：元素自带堆资源，但 ElemCopy 为 NULL
   ----------------------------------------------------------------------------
   本库容器的既定约定（见 data_type/vector.h 里 ElemCopy 的注释）：

     copy == NULL  →  该类型可以按位搬移。容器用 memcpy/memmove 搬移元素，
                       所有权随字节转移，**搬移之后不得再析构源槽位**。
     copy != NULL  →  深拷贝。源槽位仍归容器所有，搬移/擦除时正常析构。

   2026-10-03 修掉的一批缺陷全部落在这个组合上：

     - _VectorBase_Reserve / _VectorBase_ShrinkToFit
         位搬移后无条件析构旧元素 → 扩容时新数组里的指针立刻悬垂
         （use-after-free），容器析构时再 free 一次（double free）。
         这就是 Vec(PvzTracks) 崩溃的根因。
     - _VectorBase_Erase / _VectorBase_SwapErase
         位搬移后析构尾部残留的位副本 → 把已经移过去的指针 free 掉。
     - _VectorBase_Erase 的 copy == NULL 分支
         反过来漏析构被擦除的元素 → 必然泄漏。
     - _HashMapBase_Rehash
         与 Reserve 完全同型的缺陷（键和值各一份）。
     - 公开的 Rehash(n)
         n 可以小于元素个数，导致线性探测找不到空槽、死循环。

   原有测试之所以完全测不到，是因为它们只用 POD 和 String：
   POD 的 destroy 为 NULL，String 的 copy 非 NULL，两条路都绕开了这个组合。
   ============================================================================ */

/* ---------------------------------------------------------------- 探测器
   光靠"读一下 payload 看值对不对"是不够的：内存 free 之后内容往往还在，
   读出来仍是旧值，测不出 use-after-free。所以这里登记每一块活着的 payload，
   析构时校验它是否真的在册——不在册就说明这块内存已经被释放过一次了。
   这是不依赖分配器实现的精确 double-free 探测。 */

#define OWNED_MAX_LIVE 512

static int* g_live[OWNED_MAX_LIVE];
static int  g_live_count = 0;   /* 当前在册的 payload 块数 */
static int  g_alive      = 0;   /* 净存活元素数 */
static int  g_freed      = 0;   /* 析构调用次数 */
static int  g_double     = 0;   /* 释放了一块已经释放过的内存 */

static void own_register(int* p)
{
    if (p && g_live_count < OWNED_MAX_LIVE)
        g_live[g_live_count++] = p;
}

/* 返回 0 表示这块内存不在册：要么被释放过，要么根本不属于我们 */
static int own_unregister(int* p)
{
    for (int i = 0; i < g_live_count; i++)
    {
        if (g_live[i] == p)
        {
            g_live[i] = g_live[--g_live_count];
            return 1;
        }
    }
    return 0;
}

static void counters_reset(void)
{
    g_live_count = 0;
    g_alive      = 0;
    g_freed      = 0;
    g_double     = 0;
}

/* ------------------------------------------------------- 被测的元素类型 */

typedef struct
{
    int* payload;   /* 堆资源：模拟 PvzTracks 里那 9 个 Track* */
    int  id;
} Owned;

static void Owned_Create(void* addr)
{
    Owned* self = (Owned*)addr;
    self->payload = (int*)malloc(sizeof(int));
    if (self->payload) *self->payload = 0;
    self->id = 0;
    own_register(self->payload);
    g_alive++;
}

static void Owned_Destroy(void* addr)
{
    Owned* self = (Owned*)addr;
    if (self->payload)
    {
        if (!own_unregister(self->payload)) g_double++;
        free(self->payload);
        self->payload = NULL;
    }
    g_alive--;
    g_freed++;
}

/* DESTROY 非空 + COPY 为 NULL —— 出问题的就是这组配置 */
VECTOR_IMPL_EX(Owned, Owned_Create, Owned_Destroy, NULL, NULL);

/* 键用 POD i32，值是自带资源的 Owned */
HASHMAP_IMPL_EX(i32, Owned, NULL, NULL,
                NULL, NULL, NULL,
                Owned_Create, Owned_Destroy, NULL);

/* --------------------------------------------------------------- 辅助 */

static void fill_owned(Vec(Owned)* v, int n)
{
    for (int i = 0; i < n; i++)
    {
        Owned o;
        Owned_Create(&o);
        o.id = i;
        *o.payload = i * 7;
        FT(Vec(Owned), v)->push_back(v, o);
    }
}

/* 每个元素是否仍然指着"自己那一块"（id 与 payload 的内容自洽） */
static int owned_all_self_consistent(Vec(Owned)* v)
{
    for (umax i = 0; i < v->size; i++)
    {
        Owned* e = FT(Vec(Owned), v)->at(v, i);
        if (!e || !e->payload || *e->payload != e->id * 7)
            return 0;
    }
    return 1;
}

/* ======================================================= 用例 1：扩容搬移 */

static void ownership_test_vector_grow(TestRunner* r)
{
    TEST_GROUP(r, "Vec(Owned)：扩容搬移不析构源元素");
    counters_reset();
    {
        Vec(Owned) v;
        Create(Vec(Owned), &v);

        /* 1→2→4→…→64，每一步都触发一次 Reserve 的位搬移 */
        fill_owned(&v, 64);

        TEST_ASSERT_EQ(r, (int)v.size, 64);
        /* 扩容只是搬字节，不该析构任何元素；少一个就说明源槽位被误析构了 */
        TEST_ASSERT_EQ(r, g_alive, 64);
        TEST_ASSERT_EQ(r, g_double, 0);
        TEST_ASSERT_MSG(r, owned_all_self_consistent(&v), "扩容后所有元素应仍然指着自己的 payload");

        v.vptr->destroy(&v);
    }
    TEST_ASSERT_EQ(r, g_alive, 0);
    TEST_ASSERT_EQ(r, g_freed, 64);   /* 恰好每个元素析构一次 */
    TEST_ASSERT_EQ(r, g_double, 0);
}

/* ============================== 用例 2：Erase / SwapErase / ShrinkToFit */

static void ownership_test_vector_erase(TestRunner* r)
{
    TEST_GROUP(r, "Vec(Owned)：Erase / SwapErase / ShrinkToFit");
    counters_reset();
    {
        Vec(Owned) v;
        Create(Vec(Owned), &v);
        fill_owned(&v, 8);   /* 容量会先涨到 8，后面 ShrinkToFit 才有活干 */

        /* 头删：其余元素按位左移。被删的那个要析构，其余 7 个一个都不能动 */
        VCall(Vec(Owned), &v, erase, 0);
        TEST_ASSERT_EQ(r, (int)v.size, 7);
        TEST_ASSERT_EQ(r, g_alive, 7);
        TEST_ASSERT_EQ(r, g_freed, 1);
        TEST_ASSERT_EQ(r, g_double, 0);

        /* 搬进更小的缓冲 */
        VCall(Vec(Owned), &v, shrink_to_fit);
        TEST_ASSERT_EQ(r, g_alive, 7);
        TEST_ASSERT_EQ(r, g_double, 0);
        TEST_ASSERT_MSG(r, owned_all_self_consistent(&v), "ShrinkToFit 后元素应仍然指着自己的 payload");

        /* 用最后一个元素填洞：同样只该析构被覆盖的那一个 */
        VCall(Vec(Owned), &v, swap_erase, 3);
        TEST_ASSERT_EQ(r, (int)v.size, 6);
        TEST_ASSERT_EQ(r, g_alive, 6);
        TEST_ASSERT_EQ(r, g_freed, 2);
        TEST_ASSERT_EQ(r, g_double, 0);
        TEST_ASSERT_MSG(r, owned_all_self_consistent(&v), "SwapErase 后元素应仍然指着自己的 payload");

        v.vptr->destroy(&v);
    }
    TEST_ASSERT_EQ(r, g_alive, 0);
    TEST_ASSERT_EQ(r, g_freed, 8);
    TEST_ASSERT_EQ(r, g_double, 0);
}

/* ================================================== 用例 3：HashMap rehash */

static void ownership_test_hashmap_rehash(TestRunner* r)
{
    TEST_GROUP(r, "HashMap(i32, Owned)：rehash 搬移键值");
    counters_reset();
    {
        HashMap(i32, Owned) m;
        Create(HashMap(i32, Owned), &m);

        /* 容量 4→8→16→32，每一步 rehash 都要搬移已有键值 */
        for (int i = 0; i < 32; i++)
        {
            Owned v;
            Owned_Create(&v);
            v.id = i;
            *v.payload = i * 7;
            Call(HashMap(i32, Owned), &m, Insert, i, v);
        }

        TEST_ASSERT_EQ(r, (int)Call(HashMap(i32, Owned), &m, Size), 32);
        TEST_ASSERT_EQ(r, g_alive, 32);   /* rehash 不该析构任何活着的值 */
        TEST_ASSERT_EQ(r, g_double, 0);

        int intact = 1;
        for (int i = 0; i < 32; i++)
        {
            Owned* e = Call(HashMap(i32, Owned), &m, At, i);
            if (!e || !e->payload || *e->payload != e->id * 7) { intact = 0; break; }
        }
        TEST_ASSERT_MSG(r, intact, "rehash 后所有键值应保持完整");

        Call(HashMap(i32, Owned), &m, Destroy);
    }
    TEST_ASSERT_EQ(r, g_alive, 0);
    TEST_ASSERT_EQ(r, g_freed, 32);
    TEST_ASSERT_EQ(r, g_double, 0);
}

/* ====================== 用例 4：公开的 Rehash(n) 允许把表缩到比元素还小
   n < size 时新表装不下已有元素，线性探测会永远找不到空槽。
   修复方式是在 _HashMapBase_Rehash 内部把容量钳到 > size。
   ⚠ 如果这条断言挂住（不返回），说明钳位又没了——死循环正是原来的症状。 */

static void ownership_test_hashmap_rehash_too_small(TestRunner* r)
{
    TEST_GROUP(r, "HashMap(i32, Owned)：Rehash(n) 的 n 小于元素个数");
    counters_reset();
    {
        HashMap(i32, Owned) m;
        Create(HashMap(i32, Owned), &m);

        for (int i = 0; i < 8; i++)
        {
            Owned v;
            Owned_Create(&v);
            v.id = i;
            *v.payload = i * 7;
            Call(HashMap(i32, Owned), &m, Insert, i, v);
        }

        Call(HashMap(i32, Owned), &m, Rehash, 1);   /* 1 → 会被钳到能装下 8 个元素 */

        TEST_ASSERT_EQ(r, (int)Call(HashMap(i32, Owned), &m, Size), 8);
        TEST_ASSERT_EQ(r, g_alive, 8);
        TEST_ASSERT_EQ(r, g_double, 0);

        int intact = 1;
        for (int i = 0; i < 8; i++)
        {
            Owned* e = Call(HashMap(i32, Owned), &m, At, i);
            if (!e || !e->payload || *e->payload != e->id * 7) { intact = 0; break; }
        }
        TEST_ASSERT_MSG(r, intact, "钳位后的 rehash 应保留全部键值");

        Call(HashMap(i32, Owned), &m, Destroy);
    }
    TEST_ASSERT_EQ(r, g_alive, 0);
    TEST_ASSERT_EQ(r, g_freed, 8);
    TEST_ASSERT_EQ(r, g_double, 0);
}

/* ================================= 用例 5：Result 的 Destroy 必须幂等（Ok 分支）

   Ok 结果的 union 里 error 成员从未被初始化。原先 Result_Destroy 只把 ok 置为
   false，于是第二次调用会认为"E 是活的"，去 free 那块未初始化的栈内存。
   原来那条 "Destroy idempotent" 测试用的是 Err 结果，正好躲开了这一半。 */

RESULT_IMPL_EX(i32, String, NULL, NULL, _String_Destroy, _String_Copy);

static void ownership_test_result_destroy(TestRunner* r)
{
    TEST_GROUP(r, "Result(i32, String)：Destroy 幂等");
    {
        Result(i32, String) res = Result_Ok(i32, String, 42);   /* error 成员是未初始化的 */
        Result_Destroy(i32, String, &res);
        Result_Destroy(i32, String, &res);
        Result_Destroy(i32, String, &res);
        TEST_ASSERT(r, !Result_IsOk(i32, String, res));
    }
    {
        String s;
        Create(String, &s);
        Call(String, &s, Append, "boom");
        Result(i32, String) res = Result_Err(i32, String, s);
        Result_Destroy(i32, String, &res);
        Result_Destroy(i32, String, &res);
        TEST_ASSERT(r, Result_IsErr(i32, String, res));
        Call(String, &s, Destroy);
    }
}

/* ======================= 用例 6：池容器缩容后，失效的迭代器不得越界

   池容器的 Compact 会把 capacity 减半，并把活动节点重新编号到 [0, size)。
   迭代器只缓存了 pool 指针 + 槽位下标，所以缩容之后旧迭代器里的下标可能已经
   >= capacity —— 直接拿去当数组下标就是越界读，紧接着是同地址的越界写。
   修复是在各入口校验下标上界（越界按无效参数处理，不再触碰内存）。 */

static void ownership_test_pool_stale_iterator(TestRunner* r)
{
    TEST_GROUP(r, "PoolList：缩容后失效的迭代器不得越界");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        for (i32 i = 0; i < 16; i++)
            Call(PList(i32), &l, PushBack, i);       /* capacity 涨到 16 */

        PListIter(i32) it = Call(PList(i32), &l, Begin);
        for (int k = 0; k < 15; k++)
            VCall(PListIter(i32), &it, next);        /* 停在最后一个节点上 */

        for (int k = 0; k < 12; k++)
            Call(PList(i32), &l, PopFront);          /* size=4 → Compact：capacity 16→8，节点重编号 */

        Call(PList(i32), &l, EraseAfter, it);        /* 这个下标已经越界了 */
        TEST_ASSERT_EQ(r, (int)Call(PList(i32), &l, Size), 4);

        i32* bk = Call(PList(i32), &l, Back);
        TEST_ASSERT_NOT_NULL(r, bk);
        TEST_ASSERT_EQ(r, bk ? *bk : -1, 15);        /* 剩下 12..15，尾节点仍是 15 */

        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "PoolDoublyList：ShrinkToFit 不得破坏最后一个节点的 prev");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (i32 i = 0; i < 5; i++)
            Call(PDList(i32), &l, PushBack, i);

        /* new_cap 会被收到正好等于 size，此时没有任何空闲槽；
           旧实现仍然把 free list 终止符写进最后一个**活动**节点，覆盖掉它的 prev。 */
        _PoolDoublyListBase_ShrinkToFit((_PoolDoublyListBase*)&l);

        TEST_ASSERT_EQ(r, (int)Call(PDList(i32), &l, Size), 5);
        Call(PDList(i32), &l, PopBack);
        TEST_ASSERT_EQ(r, (int)Call(PDList(i32), &l, Size), 4);

        i32* bk = Call(PDList(i32), &l, Back);
        TEST_ASSERT_NOT_NULL(r, bk);                 /* 旧代码在这里 back 变成 NULL */
        TEST_ASSERT_EQ(r, bk ? *bk : -1, 3);
        i32* fr = Call(PDList(i32), &l, Front);
        TEST_ASSERT_NOT_NULL(r, fr);
        TEST_ASSERT_EQ(r, fr ? *fr : -1, 0);

        Call(PDList(i32), &l, Destroy);
    }
}

/* ------------------------------------------------------------------ 入口 */

void ownership_test(void)
{
    TEST_INIT(runner, "Ownership Tests (ElemCopy == NULL)");
    TEST_BEGIN(&runner);

    ownership_test_vector_grow(&runner);
    ownership_test_vector_erase(&runner);
    ownership_test_hashmap_rehash(&runner);
    ownership_test_hashmap_rehash_too_small(&runner);
    ownership_test_result_destroy(&runner);
    ownership_test_pool_stale_iterator(&runner);

    TEST_END(&runner);
}

#endif
