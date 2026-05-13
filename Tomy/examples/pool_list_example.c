/**
 * pool_list_example.c — 内存池化单向链表 PoolList 使用示例
 *
 * 底层为连续内存块（slot 数组），节点使用 umax 索引代替指针。
 * 删除的 slot 进入 free list 复用，自动扩容（2x）/缩容（<1/4）。
 *
 * 编译: cl pool_list_example.c /I../Tomy/include /Fe:pool_list_example.exe
 * 运行: ./pool_list_example
 */

#include "tomy.h"

int main(void)
{
    /* ========== 1. POD 类型：PList(i32) ========== */
    println("=== PList(i32) 基础操作 ===");

    PList(i32) l;
    Create(PList(i32), &l);

    /* 尾部追加 */
    Call(PList(i32), &l, PushBack, 10);
    Call(PList(i32), &l, PushBack, 20);
    Call(PList(i32), &l, PushBack, 30);
    println("size:", Call(PList(i32), &l, Size));

    /* 首尾访问 */
    i32* f = Call(PList(i32), &l, Front);
    i32* b = Call(PList(i32), &l, Back);
    println("front:", *f, "back:", *b);

    /* 头部插入与移除 */
    Call(PList(i32), &l, PushFront, 5);
    println("after push_front, front:", *Call(PList(i32), &l, Front));
    Call(PList(i32), &l, PopFront);
    println("after pop_front, front:", *Call(PList(i32), &l, Front));

    /* foreach 迭代 */
    print("foreach:");
    foreach(PList(i32), val, l)
    {
        print(" ", val);
    }
    println();

    /* BeforeBegin 哨兵模式 + InsertAfter/EraseAfter */
    PListIter(i32) bb = Call(PList(i32), &l, BeforeBegin);
    Call(PList(i32), &l, InsertAfter, bb, 1);
    print("after push_front via BeforeBegin:");
    foreach(PList(i32), val, l)
    {
        print(" ", val);
    }
    println();

    /* 反转 */
    Call(PList(i32), &l, Reverse);
    print("reversed:");
    foreach(PList(i32), val, l)
    {
        print(" ", val);
    }
    println();

    /* 手动迭代器 */
    PListIter(i32) it = VCall(PList(i32), &l, begin);
    PListIter(i32) end_it = VCall(PList(i32), &l, end);
    print("manual iterator:");
    while (!VCall(PListIter(i32), &it, equals, &end_it))
    {
        print(" ", VCall(PListIter(i32), &it, get));
        VCall(PListIter(i32), &it, next);
    }
    println();

    /* 清空 */
    Call(PList(i32), &l, Clear);
    println("after clear, is_empty:", Call(PList(i32), &l, IsEmpty));

    /* 大量 push/pop 触发自动缩容 */
    for (i32 i = 0; i < 100; i++)
        Call(PList(i32), &l, PushBack, i);
    println("after 100 pushes, size:", Call(PList(i32), &l, Size));

    for (i32 i = 0; i < 80; i++)
        Call(PList(i32), &l, PopFront);
    println("after 80 pops, size:", Call(PList(i32), &l, Size));
    println("front:", *Call(PList(i32), &l, Front));

    Call(PList(i32), &l, Destroy);

    /* ========== 2. String 类型 ========== */
    println("\n=== PList(String) ===");

    PList(String) sl;
    Create(PList(String), &sl);

    String* s1 = format("hello");
    String* s2 = format("pool");
    String* s3 = format("list");
    Call(PList(String), &sl, PushBack, *s1);
    Call(PList(String), &sl, PushBack, *s2);
    Call(PList(String), &sl, PushBack, *s3);
    Call(String, s1, Delete);
    Call(String, s2, Delete);
    Call(String, s3, Delete);

    print("String pool list:");
    foreach(PList(String), str, sl)
    {
        print(" ", &str);
    }
    println();

    Call(PList(String), &sl, Destroy);

    return 0;
}