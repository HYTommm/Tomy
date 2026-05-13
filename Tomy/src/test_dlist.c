#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef DLIST_TEST

void dlist_test_pod(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy / IsEmpty");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        TEST_ASSERT(r, Call(DList(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(DList(i32), &l, Size) == 0);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "PushFront / PushBack / Front / Back");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 1);
        Call(DList(i32), &l, PushBack, 2);
        Call(DList(i32), &l, PushFront, 0);
        TEST_ASSERT(r, Call(DList(i32), &l, Size) == 3);
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 0);
        TEST_ASSERT(r, *Call(DList(i32), &l, Back) == 2);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "PopFront / PopBack");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        for (i32 i = 1; i <= 4; i++)
            Call(DList(i32), &l, PushBack, i);
        Call(DList(i32), &l, PopFront);
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 2);
        Call(DList(i32), &l, PopBack);
        TEST_ASSERT(r, *Call(DList(i32), &l, Back) == 3);
        TEST_ASSERT(r, Call(DList(i32), &l, Size) == 2);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Insert before begin");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 2);
        Call(DList(i32), &l, PushBack, 3);
        DListIter(i32) it = VCall(DList(i32), &l, begin);
        Call(DList(i32), &l, Insert, it, 1);
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 1);
        TEST_ASSERT(r, Call(DList(i32), &l, Size) == 3);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Insert before end (appends)");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 1);
        Call(DList(i32), &l, PushBack, 3);
        /* end() = sentinel; insert before sentinel = append */
        DListIter(i32) end = VCall(DList(i32), &l, end);
        Call(DList(i32), &l, Insert, end, 2);
        TEST_ASSERT(r, *Call(DList(i32), &l, Back) == 2);
        TEST_ASSERT(r, Call(DList(i32), &l, Size) == 3);

        i32 vals[3] = { 0 };
        int idx = 0;
        foreach(DList(i32), v, l)
        {
            vals[idx++] = v;
        }
        TEST_ASSERT(r, vals[0] == 1 && vals[1] == 3 && vals[2] == 2);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Erase");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        for (i32 i = 0; i < 5; i++)
            Call(DList(i32), &l, PushBack, i);
        DListIter(i32) it = VCall(DList(i32), &l, begin);
        VCall(DListIter(i32), &it, next);
        Call(DList(i32), &l, Erase, it);
        TEST_ASSERT(r, Call(DList(i32), &l, Size) == 4);
        i32* f = Call(DList(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 0);
        /* After erasing second element (1): order is 0, 2, 3, 4 */
        int count = 0;
        foreach(DList(i32), v, l)
        {
            if (count == 1) TEST_ASSERT(r, v == 2);
            count++;
        }
        TEST_ASSERT(r, count == 4);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Clear / Re-push");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        for (i32 i = 0; i < 10; i++)
            Call(DList(i32), &l, PushBack, i);
        Call(DList(i32), &l, Clear);
        TEST_ASSERT(r, Call(DList(i32), &l, IsEmpty));
        Call(DList(i32), &l, PushBack, 99);
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 99);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Reverse");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 1);
        Call(DList(i32), &l, PushBack, 2);
        Call(DList(i32), &l, PushBack, 3);
        Call(DList(i32), &l, Reverse);
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 3);
        TEST_ASSERT(r, *Call(DList(i32), &l, Back) == 1);

        i32 vals[3] = { 0 };
        int idx = 0;
        foreach(DList(i32), v, l)
        {
            vals[idx++] = v;
        }
        TEST_ASSERT(r, vals[0] == 3 && vals[1] == 2 && vals[2] == 1);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Foreach iteration");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        for (i32 i = 0; i < 5; i++)
            Call(DList(i32), &l, PushBack, i);
        i32 sum = 0;
        foreach(DList(i32), v, l)
        {
            sum += v;
        }
        TEST_ASSERT(r, sum == 10);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Iterator manual");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 10);
        Call(DList(i32), &l, PushBack, 20);

        DListIter(i32) it = VCall(DList(i32), &l, begin);
        DListIter(i32) end_it = VCall(DList(i32), &l, end);
        TEST_ASSERT(r, !VCall(DListIter(i32), &it, equals, &end_it));
        TEST_ASSERT(r, VCall(DListIter(i32), &it, get) == 10);

        VCall(DListIter(i32), &it, next);
        TEST_ASSERT(r, VCall(DListIter(i32), &it, get) == 20);

        VCall(DListIter(i32), &it, next);
        TEST_ASSERT(r, VCall(DListIter(i32), &it, equals, &end_it));
        l.vptr->destroy(&l);
    }
}

void dlist_test_string(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy String dlist");
    {
        DList(String) l;
        Create(DList(String), &l);
        TEST_ASSERT(r, Call(DList(String), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "PushBack / Front String");
    {
        DList(String) l;
        Create(DList(String), &l);
        String* s1 = format("hello");
        String* s2 = format("world");
        Call(DList(String), &l, PushBack, *s1);
        Call(DList(String), &l, PushBack, *s2);
        Call(String, s1, Delete);
        Call(String, s2, Delete);
        String* f = Call(DList(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "hello") == 0);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Clear String dlist");
    {
        DList(String) l;
        Create(DList(String), &l);
        for (int i = 0; i < 5; i++)
        {
            String* s = format("s{}", i);
            Call(DList(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        Call(DList(String), &l, Clear);
        TEST_ASSERT(r, Call(DList(String), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Foreach String dlist");
    {
        DList(String) l;
        Create(DList(String), &l);
        for (int i = 0; i < 3; i++)
        {
            String* s = format("n{}", i);
            Call(DList(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        int count = 0;
        foreach(DList(String), str, l)
        {
            count++;
        }
        TEST_ASSERT(r, count == 3);
        l.vptr->destroy(&l);
    }
}

void dlist_test_edge(TestRunner* r)
{
    TEST_GROUP(r, "Empty ops (no crash)");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        TEST_ASSERT(r, Call(DList(i32), &l, Front) == NULL);
        TEST_ASSERT(r, Call(DList(i32), &l, Back) == NULL);
        Call(DList(i32), &l, PopFront);
        Call(DList(i32), &l, PopBack);
        Call(DList(i32), &l, Clear);
        Call(DList(i32), &l, Reverse);
        TEST_ASSERT(r, true);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Single element both ends");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 7);
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 7);
        TEST_ASSERT(r, *Call(DList(i32), &l, Back) == 7);

        DListIter(i32) it = VCall(DList(i32), &l, begin);
        DListIter(i32) end = VCall(DList(i32), &l, end);
        TEST_ASSERT(r, !VCall(DListIter(i32), &it, equals, &end));

        Call(DList(i32), &l, PopBack);
        TEST_ASSERT(r, Call(DList(i32), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "100 create/destroy cycles");
    {
        for (int i = 0; i < 100; i++)
        {
            DList(f64) l;
            Create(DList(f64), &l);
            for (f64 j = 0; j < 10; j++)
                Call(DList(f64), &l, PushBack, j);
            l.vptr->destroy(&l);
        }
        TEST_ASSERT(r, true);
    }

    TEST_GROUP(r, "Large scale (5000 elements)");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        for (i32 i = 0; i < 5000; i++)
            Call(DList(i32), &l, PushBack, i);
        TEST_ASSERT(r, Call(DList(i32), &l, Size) == 5000);
        bool ok = true;
        i32 idx = 0;
        foreach(DList(i32), v, l)
        {
            if (v != idx)
            {
                ok = false; break;
            }
            idx++;
        }
        TEST_ASSERT(r, ok);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Interleaved push/pop");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 1);
        Call(DList(i32), &l, PushFront, 0);
        Call(DList(i32), &l, PopBack);
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 0);
        TEST_ASSERT(r, *Call(DList(i32), &l, Back) == 0);
        TEST_ASSERT(r, Call(DList(i32), &l, Size) == 1);
        l.vptr->destroy(&l);
    }
}

void dlist_test(void)
{
    TEST_INIT(runner, "DoublyList Tests");
    TEST_BEGIN(&runner);

    #ifdef DLIST_TEST_POD
    dlist_test_pod(&runner);
    #endif
    #ifdef DLIST_TEST_STRING
    dlist_test_string(&runner);
    #endif
    #ifdef DLIST_TEST_EDGE
    dlist_test_edge(&runner);
    #endif

    TEST_END(&runner);
}

#endif
