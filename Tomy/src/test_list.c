#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef LIST_TEST

void list_test_pod(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy / IsEmpty");
    {
        List(i32) l;
        Create(List(i32), &l);
        TEST_ASSERT(r, Call(List(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 0);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "PushFront / Front");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushFront, 10);
        Call(List(i32), &l, PushFront, 20);
        Call(List(i32), &l, PushFront, 30);
        TEST_ASSERT(r, !Call(List(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 3);
        i32* f = Call(List(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 30);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "PushBack / Back");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 1);
        Call(List(i32), &l, PushBack, 2);
        Call(List(i32), &l, PushBack, 3);
        i32* f = Call(List(i32), &l, Front);
        i32* b = Call(List(i32), &l, Back);
        TEST_ASSERT(r, f && *f == 1);
        TEST_ASSERT(r, b && *b == 3);
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 3);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "PopFront");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 1);
        Call(List(i32), &l, PushBack, 2);
        Call(List(i32), &l, PushBack, 3);
        Call(List(i32), &l, PopFront);
        i32* f = Call(List(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 2);
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 2);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "InsertAfter / EraseAfter");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 10);
        Call(List(i32), &l, PushBack, 30);

        ListIter(i32) it = Call(List(i32), &l, Begin);
        Call(List(i32), &l, InsertAfter, it, 20);
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 3);

        i32* f = Call(List(i32), &l, Front);
        TEST_ASSERT(r, f && *f == 10);

        it = Call(List(i32), &l, Begin);
        ListIter(i32) after = it;
        Call(List(i32), &l, EraseAfter, after);
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 2);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Clear / Re-push");
    {
        List(i32) l;
        Create(List(i32), &l);
        for (i32 i = 0; i < 10; i++)
            Call(List(i32), &l, PushBack, i);
        Call(List(i32), &l, Clear);
        TEST_ASSERT(r, Call(List(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 0);

        Call(List(i32), &l, PushBack, 42);
        i32* v = Call(List(i32), &l, Front);
        TEST_ASSERT(r, v && *v == 42);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Reverse");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 1);
        Call(List(i32), &l, PushBack, 2);
        Call(List(i32), &l, PushBack, 3);
        Call(List(i32), &l, Reverse);

        TEST_ASSERT(r, *Call(List(i32), &l, Front) == 3);
        TEST_ASSERT(r, *Call(List(i32), &l, Back) == 1);
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 3);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Foreach iteration");
    {
        List(i32) l;
        Create(List(i32), &l);
        for (i32 i = 0; i < 5; i++)
            Call(List(i32), &l, PushBack, i);
        i32 sum = 0;
        foreach(List(i32), val, l)
        {
            sum += val;
        }
        TEST_ASSERT(r, sum == 10);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Iterator manual");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 100);
        Call(List(i32), &l, PushBack, 200);

        ListIter(i32) it = VCall(List(i32), &l, begin);
        ListIter(i32) end_it = VCall(List(i32), &l, end);
        TEST_ASSERT(r, !VCall(ListIter(i32), &it, equals, &end_it));

        i32 val = VCall(ListIter(i32), &it, get);
        TEST_ASSERT(r, val == 100);

        VCall(ListIter(i32), &it, next);
        val = VCall(ListIter(i32), &it, get);
        TEST_ASSERT(r, val == 200);

        VCall(ListIter(i32), &it, next);
        TEST_ASSERT(r, VCall(ListIter(i32), &it, equals, &end_it));
        l.vptr->destroy(&l);
    }
}

void list_test_string(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy String list");
    {
        List(String) l;
        Create(List(String), &l);
        TEST_ASSERT(r, Call(List(String), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "PushBack / Front String");
    {
        List(String) l;
        Create(List(String), &l);

        String* s1 = format("hello");
        String* s2 = format("world");

        Call(List(String), &l, PushBack, *s1);
        Call(List(String), &l, PushBack, *s2);

        Call(String, s1, Delete);
        Call(String, s2, Delete);

        String* f = Call(List(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "hello") == 0);
        String* b = Call(List(String), &l, Back);
        TEST_ASSERT(r, b && strcmp(b->data, "world") == 0);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "PushFront / PopFront String");
    {
        List(String) l;
        Create(List(String), &l);

        String* s = format("front");
        Call(List(String), &l, PushFront, *s);
        Call(String, s, Delete);

        String* f = Call(List(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "front") == 0);
        TEST_ASSERT(r, Call(List(String), &l, Size) == 1);

        Call(List(String), &l, PopFront);
        TEST_ASSERT(r, Call(List(String), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Clear String list");
    {
        List(String) l;
        Create(List(String), &l);
        for (int i = 0; i < 5; i++)
        {
            String* s = format("str_{}", i);
            Call(List(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        TEST_ASSERT(r, Call(List(String), &l, Size) == 5);
        Call(List(String), &l, Clear);
        TEST_ASSERT(r, Call(List(String), &l, IsEmpty));

        String* s = format("after_clear");
        Call(List(String), &l, PushBack, *s);
        Call(String, s, Delete);

        String* f = Call(List(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "after_clear") == 0);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Foreach String");
    {
        List(String) l;
        Create(List(String), &l);
        for (int i = 0; i < 3; i++)
        {
            String* s = format("n{}", i);
            Call(List(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        int count = 0;
        foreach(List(String), str, l)
        {
            count++;
        }
        TEST_ASSERT(r, count == 3);
        l.vptr->destroy(&l);
    }
}

void list_test_edge(TestRunner* r)
{
    TEST_GROUP(r, "Empty list ops (no crash)");
    {
        List(i32) l;
        Create(List(i32), &l);
        TEST_ASSERT(r, Call(List(i32), &l, Front) == NULL);
        TEST_ASSERT(r, Call(List(i32), &l, Back) == NULL);
        Call(List(i32), &l, PopFront);
        Call(List(i32), &l, Clear);
        Call(List(i32), &l, Reverse);
        TEST_ASSERT(r, true);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Single element");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 42);
        TEST_ASSERT(r, *Call(List(i32), &l, Front) == 42);
        TEST_ASSERT(r, *Call(List(i32), &l, Back) == 42);
        Call(List(i32), &l, PopFront);
        TEST_ASSERT(r, Call(List(i32), &l, IsEmpty));
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 0);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "100 create/destroy cycles");
    {
        for (int i = 0; i < 100; i++)
        {
            List(f64) l;
            Create(List(f64), &l);
            for (f64 j = 0; j < 10; j++)
                Call(List(f64), &l, PushBack, j);
            l.vptr->destroy(&l);
        }
        TEST_ASSERT(r, true);
    }

    TEST_GROUP(r, "BeforeBegin / InsertAfter combo");
    {
        List(i32) l;
        Create(List(i32), &l);
        ListIter(i32) bb = Call(List(i32), &l, BeforeBegin);
        Call(List(i32), &l, InsertAfter, bb, 1);
        Call(List(i32), &l, InsertAfter, bb, 2);
        /* BeforeBegin → InsertAfter inserts at front, so: 2, 1 */
        TEST_ASSERT(r, *Call(List(i32), &l, Front) == 2);
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 2);

        Call(List(i32), &l, EraseAfter, bb);
        TEST_ASSERT(r, *Call(List(i32), &l, Front) == 1);
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 1);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Reverse single element");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 1);
        Call(List(i32), &l, Reverse);
        TEST_ASSERT(r, *Call(List(i32), &l, Front) == 1);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "Large scale (5000 elements)");
    {
        List(i32) l;
        Create(List(i32), &l);
        for (i32 i = 0; i < 5000; i++)
            Call(List(i32), &l, PushBack, i);
        TEST_ASSERT(r, Call(List(i32), &l, Size) == 5000);

        bool ok = true;
        i32 idx = 0;
        foreach(List(i32), val, l)
        {
            if (val != idx)
            {
                ok = false; break;
            }
            idx++;
        }
        TEST_ASSERT(r, ok);
        l.vptr->destroy(&l);
    }
}

void list_test(void)
{
    TEST_INIT(runner, "List Tests");
    TEST_BEGIN(&runner);

    #ifdef LIST_TEST_POD
    list_test_pod(&runner);
    #endif
    #ifdef LIST_TEST_STRING
    list_test_string(&runner);
    #endif
    #ifdef LIST_TEST_EDGE
    list_test_edge(&runner);
    #endif

    TEST_END(&runner);
}

#endif
