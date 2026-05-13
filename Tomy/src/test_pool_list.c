#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef POOLLIST_TEST

void pool_list_test_pod(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy / IsEmpty");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        TEST_ASSERT(r, Call(PList(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 0);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "PushFront / Front");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        Call(PList(i32), &l, PushFront, 10);
        Call(PList(i32), &l, PushFront, 20);
        Call(PList(i32), &l, PushFront, 30);
        TEST_ASSERT(r, !Call(PList(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 3);
        i32* f = Call(PList(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 30);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "PushBack / Back");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        Call(PList(i32), &l, PushBack, 1);
        Call(PList(i32), &l, PushBack, 2);
        Call(PList(i32), &l, PushBack, 3);
        i32* f = Call(PList(i32), &l, Front);
        i32* b = Call(PList(i32), &l, Back);
        TEST_ASSERT(r, f && *f == 1);
        TEST_ASSERT(r, b && *b == 3);
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 3);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "PopFront");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        Call(PList(i32), &l, PushBack, 1);
        Call(PList(i32), &l, PushBack, 2);
        Call(PList(i32), &l, PushBack, 3);
        Call(PList(i32), &l, PopFront);
        i32* f = Call(PList(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 2);
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 2);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "InsertAfter / EraseAfter");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        Call(PList(i32), &l, PushBack, 10);
        Call(PList(i32), &l, PushBack, 30);

        PListIter(i32) it = Call(PList(i32), &l, Begin);
        Call(PList(i32), &l, InsertAfter, it, 20);
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 3);

        i32* f = Call(PList(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 10);

        it = Call(PList(i32), &l, Begin);
        PListIter(i32) after = it;
        Call(PList(i32), &l, EraseAfter, after);
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 2);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Clear / Re-push");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        for (i32 i = 0; i < 10; i++)
            Call(PList(i32), &l, PushBack, i);
        Call(PList(i32), &l, Clear);
        TEST_ASSERT(r, Call(PList(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 0);

        Call(PList(i32), &l, PushBack, 42);
        i32* v = Call(PList(i32), &l, Front);
        TEST_ASSERT(r, v && *v == 42);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Reverse");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        Call(PList(i32), &l, PushBack, 1);
        Call(PList(i32), &l, PushBack, 2);
        Call(PList(i32), &l, PushBack, 3);
        Call(PList(i32), &l, Reverse);

        TEST_ASSERT(r, *Call(PList(i32), &l, Front) == 3);
        TEST_ASSERT(r, *Call(PList(i32), &l, Back) == 1);
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 3);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Foreach iteration");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        for (i32 i = 0; i < 5; i++)
            Call(PList(i32), &l, PushBack, i);
        i32 sum = 0;
        foreach(PList(i32), val, l)
        {
            sum += val;
        }
        TEST_ASSERT(r, sum == 10);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Iterator manual");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        Call(PList(i32), &l, PushBack, 100);
        Call(PList(i32), &l, PushBack, 200);

        PListIter(i32) it = VCall(PList(i32), &l, begin);
        PListIter(i32) end_it = VCall(PList(i32), &l, end);
        TEST_ASSERT(r, !VCall(PListIter(i32), &it, equals, &end_it));

        i32 val = VCall(PListIter(i32), &it, get);
        TEST_ASSERT(r, val == 100);

        VCall(PListIter(i32), &it, next);
        val = VCall(PListIter(i32), &it, get);
        TEST_ASSERT(r, val == 200);

        VCall(PListIter(i32), &it, next);
        TEST_ASSERT(r, VCall(PListIter(i32), &it, equals, &end_it));
        Call(PList(i32), &l, Destroy);
    }
}

void pool_list_test_string(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy String list");
    {
        PList(String) l;
        Create(PList(String), &l);
        TEST_ASSERT(r, Call(PList(String), &l, IsEmpty));
        Call(PList(String), &l, Destroy);
    }

    TEST_GROUP(r, "PushBack / Front String");
    {
        PList(String) l;
        Create(PList(String), &l);

        String* s1 = format("hello");
        String* s2 = format("world");

        Call(PList(String), &l, PushBack, *s1);
        Call(PList(String), &l, PushBack, *s2);

        Call(String, s1, Delete);
        Call(String, s2, Delete);

        String* f = Call(PList(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "hello") == 0);
        String* b = Call(PList(String), &l, Back);
        TEST_ASSERT(r, b && strcmp(b->data, "world") == 0);
        Call(PList(String), &l, Destroy);
    }

    TEST_GROUP(r, "PushFront / PopFront String");
    {
        PList(String) l;
        Create(PList(String), &l);

        String* s = format("front");
        Call(PList(String), &l, PushFront, *s);
        Call(String, s, Delete);

        String* f = Call(PList(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "front") == 0);
        TEST_ASSERT(r, Call(PList(String), &l, Size) == 1);

        Call(PList(String), &l, PopFront);
        TEST_ASSERT(r, Call(PList(String), &l, IsEmpty));
        Call(PList(String), &l, Destroy);
    }

    TEST_GROUP(r, "Clear String list");
    {
        PList(String) l;
        Create(PList(String), &l);
        for (int i = 0; i < 5; i++)
        {
            String* s = format("str_{}", i);
            Call(PList(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        TEST_ASSERT(r, Call(PList(String), &l, Size) == 5);
        Call(PList(String), &l, Clear);
        TEST_ASSERT(r, Call(PList(String), &l, IsEmpty));

        String* s = format("after_clear");
        Call(PList(String), &l, PushBack, *s);
        Call(String, s, Delete);

        String* f = Call(PList(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "after_clear") == 0);
        Call(PList(String), &l, Destroy);
    }
}

void pool_list_test_edge(TestRunner* r)
{
    TEST_GROUP(r, "Empty list ops (no crash)");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        TEST_ASSERT(r, Call(PList(i32), &l, Front) == NULL);
        TEST_ASSERT(r, Call(PList(i32), &l, Back) == NULL);
        Call(PList(i32), &l, PopFront);
        Call(PList(i32), &l, Clear);
        Call(PList(i32), &l, Reverse);
        TEST_ASSERT(r, true);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Single element");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        Call(PList(i32), &l, PushBack, 42);
        TEST_ASSERT(r, *Call(PList(i32), &l, Front) == 42);
        TEST_ASSERT(r, *Call(PList(i32), &l, Back) == 42);
        Call(PList(i32), &l, PopFront);
        TEST_ASSERT(r, Call(PList(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 0);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Push/pop cycle (slot reuse)");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        for (int cycle = 0; cycle < 5; cycle++)
        {
            for (i32 j = 0; j < 10; j++)
                Call(PList(i32), &l, PushBack, j);
            TEST_ASSERT(r, Call(PList(i32), &l, Size) == 10);
            for (i32 j = 0; j < 10; j++)
                Call(PList(i32), &l, PopFront);
            TEST_ASSERT(r, Call(PList(i32), &l, IsEmpty));
        }
        TEST_ASSERT(r, true);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "100 create/destroy cycles");
    {
        for (int i = 0; i < 100; i++)
        {
            PList(f64) l;
            Create(PList(f64), &l);
            for (f64 j = 0; j < 10; j++)
                Call(PList(f64), &l, PushBack, j);
            Call(PList(f64), &l, Destroy);
        }
        TEST_ASSERT(r, true);
    }

    TEST_GROUP(r, "BeforeBegin / InsertAfter combo");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        PListIter(i32) bb = Call(PList(i32), &l, BeforeBegin);
        Call(PList(i32), &l, InsertAfter, bb, 1);
        Call(PList(i32), &l, InsertAfter, bb, 2);
        /* BeforeBegin → InsertAfter = PushFront, so: 2, 1 */
        TEST_ASSERT(r, *Call(PList(i32), &l, Front) == 2);
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 2);

        Call(PList(i32), &l, EraseAfter, bb);
        TEST_ASSERT(r, *Call(PList(i32), &l, Front) == 1);
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 1);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Reverse single element");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        Call(PList(i32), &l, PushBack, 1);
        Call(PList(i32), &l, Reverse);
        TEST_ASSERT(r, *Call(PList(i32), &l, Front) == 1);
        Call(PList(i32), &l, Destroy);
    }

    TEST_GROUP(r, "Large scale (5000 elements)");
    {
        PList(i32) l;
        Create(PList(i32), &l);
        for (i32 i = 0; i < 5000; i++)
            Call(PList(i32), &l, PushBack, i);
        TEST_ASSERT(r, Call(PList(i32), &l, Size) == 5000);

        bool ok = true;
        i32 idx = 0;
        foreach(PList(i32), val, l)
        {
            if (val != idx) { ok = false; break; }
            idx++;
        }
        TEST_ASSERT(r, ok);
        Call(PList(i32), &l, Destroy);
    }
}

void pool_list_test(void)
{
    TEST_INIT(runner, "PoolList Tests");
    TEST_BEGIN(&runner);

#ifdef POOLLIST_TEST_POD
    pool_list_test_pod(&runner);
#endif
#ifdef POOLLIST_TEST_STRING
    pool_list_test_string(&runner);
#endif
#ifdef POOLLIST_TEST_EDGE
    pool_list_test_edge(&runner);
#endif

    TEST_END(&runner);
}

#endif
