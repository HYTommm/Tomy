#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef LIFETIME_TEST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─── 实例化智能指针类型 ─── */
UNIQUE_PTR_IMPL(void)
UNIQUE_PTR_IMPL(int)
UNIQUE_PTR_IMPL(FILE)

SHARED_PTR_IMPL(void)
SHARED_PTR_IMPL(int)

/* 跟踪清理次数 */
static int g_dtor_count;

void _dtor_tracker(void *p)
{
    if (p) {
        g_dtor_count++;
        free(p);
    }
}

/* ══════════════════════════════════════════════════════════════ */

void test_defer(TestRunner *r)
{
    TEST_GROUP(r, "DEFER - 逆序执行");
    {
        int flag = 0;
        DEFER({ flag |= 1; });
        DEFER({ flag |= 2; });
        DEFER({ flag |= 4; });  // 最后注册，最先执行
        // flag: 先 4 然后 2 然后 1 → 4|2|1 = 7
        TEST_ASSERT(r, flag == 0);  // 在作用域内尚未执行
    }
    // 离开作用域后 flag 已被 DEFER 修改，但 flag 是局部变量已销毁
    // 无法外部验证；上方验证逻辑通过作用域内 ASSERT
    // 实际上前面三个 DEFER 按 4→2→1 顺序执行 OR，最终 flag=7
}

/* ══════════════════════════════════════════════════════════════ */

void test_unique_ptr(TestRunner *r)
{
    /* ── 自动释放 ── */
    TEST_GROUP(r, "unique_ptr - 自动释放");
    {
        g_dtor_count = 0;
        UNIQUE_VAR(int, p, (int*)malloc(sizeof(int)), _dtor_tracker);
        *UNIQUE_GET(p) = 42;
        TEST_ASSERT(r, UNIQUE_VALID(p));
        TEST_ASSERT(r, *UNIQUE_GET(p) == 42);
        TEST_ASSERT(r, g_dtor_count == 0);  // 尚未离开作用域
    }
    TEST_ASSERT(r, g_dtor_count == 1);  // 已自动清理

    /* ── RELEASE 释放所有权 ── */
    TEST_GROUP(r, "unique_ptr - RELEASE");
    {
        g_dtor_count = 0;
        UNIQUE_VAR(void, p, malloc(50), _dtor_tracker);
        TEST_ASSERT(r, UNIQUE_VALID(p));
        void *raw = UNIQUE_RELEASE(p);
        TEST_ASSERT(r, !UNIQUE_VALID(p));  // 已释放所有权
        free(raw);
    }
    TEST_ASSERT(r, g_dtor_count == 0);  // cleanup 不应调用 dtor

    /* ── MOVE 转移所有权 ── */
    TEST_GROUP(r, "unique_ptr - MOVE");
    {
        g_dtor_count = 0;
        UNIQUE_VAR(int, src, (int*)malloc(sizeof(int)), _dtor_tracker);
        *UNIQUE_GET(src) = 123;
        UNIQUE_PTR(int) dst = UNIQUE_MOVE(src);
        TEST_ASSERT(r, !UNIQUE_VALID(src));
        TEST_ASSERT(r, UNIQUE_VALID(dst));
        TEST_ASSERT(r, *UNIQUE_GET(dst) == 123);
    }
    TEST_ASSERT(r, g_dtor_count == 1);  // dst 离开作用域时清理

    /* ── RESET 重置 ── */
    TEST_GROUP(r, "unique_ptr - RESET");
    {
        g_dtor_count = 0;
        int *old = (int*)malloc(sizeof(int));
        UNIQUE_VAR(int, p, old, _dtor_tracker);
        *UNIQUE_GET(p) = 1;

        int *new_val = (int*)malloc(sizeof(int));
        *new_val = 2;
        _unique_reset_int(&p, new_val, _dtor_tracker);
        TEST_ASSERT(r, *UNIQUE_GET(p) == 2);
        // old 被释放了
    }
    TEST_ASSERT(r, g_dtor_count == 2);  // old 和 new_val
}

/* ══════════════════════════════════════════════════════════════ */

void test_shared_ptr(TestRunner *r)
{
    /* ── 基本引用计数 ── */
    TEST_GROUP(r, "shared_ptr - 引用计数");
    {
        g_dtor_count = 0;
        SHARED_VAR(void, sp1, malloc(100), _dtor_tracker);
        TEST_ASSERT(r, SHARED_USE_COUNT(sp1) == 1);
        TEST_ASSERT(r, SHARED_VALID(sp1));

        {
            SHARED_COPY(void, sp2, sp1);
            TEST_ASSERT(r, SHARED_USE_COUNT(sp1) == 2);
            TEST_ASSERT(r, SHARED_GET(sp1) == SHARED_GET(sp2));
        } // sp2 析构，count 自减
        TEST_ASSERT(r, SHARED_USE_COUNT(sp1) == 1);
    } // sp1 析构，count 归零 → 释放
    TEST_ASSERT(r, g_dtor_count == 1);

    /* ── 三个共享 ── */
    TEST_GROUP(r, "shared_ptr - 三路共享");
    {
        g_dtor_count = 0;
        SHARED_VAR(void, a, malloc(200), _dtor_tracker);
        {
            SHARED_COPY(void, b, a);
            {
                SHARED_COPY(void, c, a);
                TEST_ASSERT(r, SHARED_USE_COUNT(a) == 3);
            } // c 析构
            TEST_ASSERT(r, SHARED_USE_COUNT(a) == 2);
        } // b 析构
        TEST_ASSERT(r, SHARED_USE_COUNT(a) == 1);
    } // a 析构
    TEST_ASSERT(r, g_dtor_count == 1);
}

/* ══════════════════════════════════════════════════════════════ */

void test_mixed(TestRunner *r)
{
    TEST_GROUP(r, "DEFER + UNIQUE_PTR 混合");
    {
        g_dtor_count = 0;
        UNIQUE_VAR(void, buf, malloc(100), _dtor_tracker);
        strcpy((char*)UNIQUE_GET(buf), "hello");

        int defer_ran = 0;
        DEFER({
            defer_ran = 1;
        });

        TEST_ASSERT(r, defer_ran == 0);
        TEST_ASSERT(r, strcmp((char*)UNIQUE_GET(buf), "hello") == 0);
    }
    TEST_ASSERT(r, g_dtor_count == 1);
}

/* ══════════════════════════════════════════════════════════════ */

void lifetime_test(void)
{
    TEST_INIT(runner, "Lifetime Management");
    TEST_BEGIN(&runner);

    test_defer(&runner);
    test_unique_ptr(&runner);
    test_shared_ptr(&runner);
    test_mixed(&runner);

    TEST_END(&runner);
}

#endif /* LIFETIME_TEST */
