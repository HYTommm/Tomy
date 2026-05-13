#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef POOLDList_TEST

void pool_dlist_test_pod(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy / IsEmpty");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        TEST_ASSERT(r, Call(PDList(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 0);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "PushFront / Front");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        Call(PDList(i32), &l, PushFront, 10);
        Call(PDList(i32), &l, PushFront, 20);
        Call(PDList(i32), &l, PushFront, 30);
        TEST_ASSERT(r, !Call(PDList(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 3);
        i32* f = Call(PDList(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 30);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "PushBack / Back");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        Call(PDList(i32), &l, PushBack, 1);
        Call(PDList(i32), &l, PushBack, 2);
        Call(PDList(i32), &l, PushBack, 3);
        i32* f = Call(PDList(i32), &l, Front);
        i32* b = Call(PDList(i32), &l, Back);
        TEST_ASSERT(r, f && *f == 1);
        TEST_ASSERT(r, b && *b == 3);
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 3);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "PopFront");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        Call(PDList(i32), &l, PushBack, 1);
        Call(PDList(i32), &l, PushBack, 2);
        Call(PDList(i32), &l, PushBack, 3);
        Call(PDList(i32), &l, PopFront);
        i32* f = Call(PDList(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 2);
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 2);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "PopBack");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        Call(PDList(i32), &l, PushBack, 1);
        Call(PDList(i32), &l, PushBack, 2);
        Call(PDList(i32), &l, PushBack, 3);
        Call(PDList(i32), &l, PopBack);
        i32* b = Call(PDList(i32), &l, Back);
        TEST_ASSERT(r, b && *b == 2);
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 2);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Insert / Erase");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        Call(PDList(i32), &l, PushBack, 10);
        Call(PDList(i32), &l, PushBack, 30);

        PDListIter(i32) it = Call(PDList(i32), &l, Begin);
        VCall(PDListIter(i32), &it, next);
        Call(PDList(i32), &l, Insert, it, 20);
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 3);

        /* order: 10, 20, 30 */
        i32* f = Call(PDList(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 10);

        it = Call(PDList(i32), &l, Begin);
        VCall(PDListIter(i32), &it, next);
        Call(PDList(i32), &l, Erase, it);
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 2);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Clear / Re-push");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (i32 i = 0; i < 10; i++)
            Call(PDList(i32), &l, PushBack, i);
        Call(PDList(i32), &l, Clear);
        TEST_ASSERT(r, Call(PDList(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 0);

        Call(PDList(i32), &l, PushBack, 42);
        i32* v = Call(PDList(i32), &l, Front);
        TEST_ASSERT(r, v && *v == 42);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Reverse");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        Call(PDList(i32), &l, PushBack, 1);
        Call(PDList(i32), &l, PushBack, 2);
        Call(PDList(i32), &l, PushBack, 3);
        Call(PDList(i32), &l, Reverse);

        TEST_ASSERT(r, *Call(PDList(i32), &l, Front) == 3);
        TEST_ASSERT(r, *Call(PDList(i32), &l, Back) == 1);
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 3);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Foreach iteration");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (i32 i = 0; i < 5; i++)
            Call(PDList(i32), &l, PushBack, i);
        i32 sum = 0;
        foreach(PDList(i32), val, l)
        {
            sum += val;
        }
        TEST_ASSERT(r, sum == 10);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Iterator manual");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        Call(PDList(i32), &l, PushBack, 100);
        Call(PDList(i32), &l, PushBack, 200);

        PDListIter(i32) it = VCall(PDList(i32), &l, begin);
        PDListIter(i32) end_it = VCall(PDList(i32), &l, end);
        TEST_ASSERT(r, !VCall(PDListIter(i32), &it, equals, &end_it));

        i32 val = VCall(PDListIter(i32), &it, get);
        TEST_ASSERT(r, val == 100);

        VCall(PDListIter(i32), &it, next);
        val = VCall(PDListIter(i32), &it, get);
        TEST_ASSERT(r, val == 200);

        VCall(PDListIter(i32), &it, next);
        TEST_ASSERT(r, VCall(PDListIter(i32), &it, equals, &end_it));
        Call(PDList(i32), &l, Destroy);
    }
}

void pool_dlist_test_string(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy String list");
    {
        PDList(String) l;
        Create(PDList(String), &l);
        TEST_ASSERT(r, Call(PDList(String), &l, IsEmpty));
        Call(PDList(String), &l, Destroy);
    }

    TEST_GROUP(r, "PushBack / Front String");
    {
        PDList(String) l;
        Create(PDList(String), &l);

        String* s1 = format("hello");
        String* s2 = format("world");

        Call(PDList(String), &l, PushBack, *s1);
        Call(PDList(String), &l, PushBack, *s2);

        Call(String, s1, Delete);
        Call(String, s2, Delete);

        String* f = Call(PDList(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "hello") == 0);
        String* b = Call(PDList(String), &l, Back);
        TEST_ASSERT(r, b && strcmp(b->data, "world") == 0);
        Call(PDList(String), &l, Destroy);
    }

    TEST_GROUP(r, "PushFront / PopFront String");
    {
        PDList(String) l;
        Create(PDList(String), &l);

        String* s = format("front");
        Call(PDList(String), &l, PushFront, *s);
        Call(String, s, Delete);

        String* f = Call(PDList(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "front") == 0);
        TEST_ASSERT(r, Call(PDList(String), &l, Size) == 1);

        Call(PDList(String), &l, PopFront);
        TEST_ASSERT(r, Call(PDList(String), &l, IsEmpty));
        Call(PDList(String), &l, Destroy);
    }

    TEST_GROUP(r, "Clear String list");
    {
        PDList(String) l;
        Create(PDList(String), &l);
        for (int i = 0; i < 5; i++)
        {
            String* s = format("str_{}", i);
            Call(PDList(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        TEST_ASSERT(r, Call(PDList(String), &l, Size) == 5);
        Call(PDList(String), &l, Clear);
        TEST_ASSERT(r, Call(PDList(String), &l, IsEmpty));

        String* s = format("after_clear");
        Call(PDList(String), &l, PushBack, *s);
        Call(String, s, Delete);

        String* f = Call(PDList(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "after_clear") == 0);
        Call(PDList(String), &l, Destroy);
    }
}

void pool_dlist_test_edge(TestRunner* r)
{
    TEST_GROUP(r, "Empty list ops (no crash)");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        TEST_ASSERT(r, Call(PDList(i32), &l, Front) == NULL);
        TEST_ASSERT(r, Call(PDList(i32), &l, Back) == NULL);
        Call(PDList(i32), &l, PopFront);
        Call(PDList(i32), &l, PopBack);
        Call(PDList(i32), &l, Clear);
        Call(PDList(i32), &l, Reverse);
        TEST_ASSERT(r, true);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Single element");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        Call(PDList(i32), &l, PushBack, 42);
        TEST_ASSERT(r, *Call(PDList(i32), &l, Front) == 42);
        TEST_ASSERT(r, *Call(PDList(i32), &l, Back) == 42);
        Call(PDList(i32), &l, PopFront);
        TEST_ASSERT(r, Call(PDList(i32), &l, IsEmpty));
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Push/pop cycle (slot reuse)");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (int cycle = 0; cycle < 5; cycle++)
        {
            for (i32 j = 0; j < 10; j++)
                Call(PDList(i32), &l, PushBack, j);
            TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 10);
            for (i32 j = 0; j < 10; j++)
                Call(PDList(i32), &l, PopFront);
            TEST_ASSERT(r, Call(PDList(i32), &l, IsEmpty));
        }
        TEST_ASSERT(r, true);
        Call(PDList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "100 create/destroy cycles");
    {
        for (int i = 0; i < 100; i++)
        {
            PDList(f64) l;
            Create(PDList(f64), &l);
            for (f64 j = 0; j < 10; j++)
                Call(PDList(f64), &l, PushBack, j);
            Call(PDList(f64), &l, Destroy);
        }
        TEST_ASSERT(r, true);
    }

    TEST_GROUP(r, "Large scale (5000 elements)");
    {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (i32 i = 0; i < 5000; i++)
            Call(PDList(i32), &l, PushBack, i);
        TEST_ASSERT(r, Call(PDList(i32), &l, Size) == 5000);

        bool ok = true;
        i32 idx = 0;
        foreach(PDList(i32), val, l)
        {
            if (val != idx) { ok = false; break; }
            idx++;
        }
        TEST_ASSERT(r, ok);
        Call(PDList(i32), &l, Destroy);
    }
}

void pool_dlist_test(void)
{
    TEST_INIT(runner, "PoolDoublyList Tests");
    TEST_BEGIN(&runner);

#ifdef POOLDList_TEST_POD
    pool_dlist_test_pod(&runner);
#endif
#ifdef POOLDList_TEST_STRING
    pool_dlist_test_string(&runner);
#endif
#ifdef POOLDList_TEST_EDGE
    pool_dlist_test_edge(&runner);
#endif

    TEST_END(&runner);
}

#endif
