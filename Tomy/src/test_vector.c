#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef VECTOR_TEST

void vector_test_pod(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy / IsEmpty");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        TEST_ASSERT(r, VCall(Vec(f32), &v, is_empty));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "PushBack / At");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->push_back(&v, 10.0f);
        FT(Vec(f32), &v)->push_back(&v, 20.0f);
        FT(Vec(f32), &v)->push_back(&v, 30.0f);
        TEST_ASSERT(r, !VCall(Vec(f32), &v, is_empty));

        f32* a0 = FT(Vec(f32), &v)->at(&v, 0);
        f32* a1 = FT(Vec(f32), &v)->at(&v, 1);
        f32* a2 = FT(Vec(f32), &v)->at(&v, 2);
        TEST_ASSERT(r, *a0 == 10.0f && *a1 == 20.0f && *a2 == 30.0f);

        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Front / Back");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->push_back(&v, 1.0f);
        FT(Vec(f32), &v)->push_back(&v, 2.0f);
        FT(Vec(f32), &v)->push_back(&v, 3.0f);
        f32* f = FT(Vec(f32), &v)->front(&v);
        f32* b = FT(Vec(f32), &v)->back(&v);
        TEST_ASSERT(r, *f == 1.0f && *b == 3.0f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "PopBack");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->push_back(&v, 1.0f);
        FT(Vec(f32), &v)->push_back(&v, 2.0f);
        FT(Vec(f32), &v)->push_back(&v, 3.0f);
        FT(Vec(f32), &v)->pop_back(&v);
        f32* b = FT(Vec(f32), &v)->back(&v);
        TEST_ASSERT(r, *b == 2.0f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Erase");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 1; i <= 5; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        FT(Vec(f32), &v)->erase(&v, 1);
        f32* a0 = FT(Vec(f32), &v)->at(&v, 0);
        f32* a1 = FT(Vec(f32), &v)->at(&v, 1);
        TEST_ASSERT(r, *a0 == 1.0f && *a1 == 3.0f);

        FT(Vec(f32), &v)->erase(&v, 3);
        f32* b = FT(Vec(f32), &v)->back(&v);
        TEST_ASSERT(r, *b == 4.0f);

        FT(Vec(f32), &v)->erase(&v, 0);
        f32* f = FT(Vec(f32), &v)->front(&v);
        TEST_ASSERT(r, *f == 3.0f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Reserve");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->reserve(&v, 1000);
        TEST_ASSERT(r, true);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Clear / Re-push");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 0; i < 10; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        FT(Vec(f32), &v)->clear(&v);
        TEST_ASSERT(r, VCall(Vec(f32), &v, is_empty));

        FT(Vec(f32), &v)->push_back(&v, 42.0f);
        f32* a = FT(Vec(f32), &v)->at(&v, 0);
        TEST_ASSERT(r, *a == 42.0f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Foreach iteration");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 0; i < 5; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        f32 sum = 0;
        foreach(Vec(f32), num, v)
        {
            sum += num;
        }
        TEST_ASSERT(r, sum == 10.0f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Iterator");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->push_back(&v, 100.0f);
        FT(Vec(f32), &v)->push_back(&v, 200.0f);

        VecIter(f32) it = VCall(Vec(f32), &v, begin);
        VecIter(f32) end_it = VCall(Vec(f32), &v, end);
        TEST_ASSERT(r, !VCall(VecIter(f32), &it, equals, &end_it));

        f32 val = VCall(VecIter(f32), &it, get);
        TEST_ASSERT(r, val == 100.0f);

        VCall(VecIter(f32), &it, next);
        val = VCall(VecIter(f32), &it, get);
        TEST_ASSERT(r, val == 200.0f);

        VCall(VecIter(f32), &it, next);
        TEST_ASSERT(r, VCall(VecIter(f32), &it, equals, &end_it));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Large scale (10000 elements)");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (int i = 0; i < 10000; i++)
            FT(Vec(f32), &v)->push_back(&v, (f32)i);

        bool ok = true;
        for (int i = 0; i < 10000; i++)
        {
            f32* val = FT(Vec(f32), &v)->at(&v, i);
            if (*val != (f32)i)
            {
                ok = false; break;
            }
        }
        TEST_ASSERT(r, ok);

        f32* data = FT(Vec(f32), &v)->data(&v);
        TEST_ASSERT(r, data[0] == 0.0f && data[9999] == 9999.0f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "EmplaceBack POD");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        f32* p = (f32*)FT(Vec(f32), &v)->emplace_back(&v);
        TEST_ASSERT(r, p != NULL && !VCall(Vec(f32), &v, is_empty));
        *p = 3.14f;
        f32* b = FT(Vec(f32), &v)->back(&v);
        TEST_ASSERT(r, *b == 3.14f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "SwapErase POD");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 0; i < 4; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        // [0, 1, 2, 3] — swap_erase(1) → last element (3) moves to 1 → [0, 3, 2]
        FT(Vec(f32), &v)->swap_erase(&v, 1);
        f32* a0 = FT(Vec(f32), &v)->at(&v, 0);
        f32* a1 = FT(Vec(f32), &v)->at(&v, 1);
        TEST_ASSERT(r, *a0 == 0.0f && *a1 == 3.0f);
        f32* b = FT(Vec(f32), &v)->back(&v);
        TEST_ASSERT(r, *b == 2.0f);
        v.vptr->destroy(&v);
    }
}

void vector_test_string(TestRunner* r)
{
    TEST_GROUP(r, "Create / Destroy");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        TEST_ASSERT(r, VCall(Vec(String), &v, is_empty));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "PushBack / At");
    {
        Vec(String) v;
        Create(Vec(String), &v);

        String* s1 = format("hello");
        String* s2 = format("world");
        String* s3 = format("test");

        FT(Vec(String), &v)->push_back(&v, *s1);
        FT(Vec(String), &v)->push_back(&v, *s2);
        FT(Vec(String), &v)->push_back(&v, *s3);

        Call(String, s1, Delete);
        Call(String, s2, Delete);
        Call(String, s3, Delete);

        const String* a0 = FT(Vec(String), &v)->at(&v, 0);
        const String* a1 = FT(Vec(String), &v)->at(&v, 1);
        const String* a2 = FT(Vec(String), &v)->at(&v, 2);
        TEST_ASSERT(r, strcmp(a0->data, "hello") == 0);
        TEST_ASSERT(r, strcmp(a1->data, "world") == 0);
        TEST_ASSERT(r, strcmp(a2->data, "test") == 0);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Clear");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        for (int i = 0; i < 10; i++)
        {
            String* s = format("str_{}", i);
            FT(Vec(String), &v)->push_back(&v, *s);
            Call(String, s, Delete);
        }
        FT(Vec(String), &v)->clear(&v);
        TEST_ASSERT(r, VCall(Vec(String), &v, is_empty));

        String* s = format("after_clear");
        FT(Vec(String), &v)->push_back(&v, *s);
        Call(String, s, Delete);
        const String* a = FT(Vec(String), &v)->at(&v, 0);
        TEST_ASSERT(r, strcmp(a->data, "after_clear") == 0);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Erase string");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        const char* words[] = { "aaa", "bbb", "ccc", "ddd" };
        for (int i = 0; i < 4; i++)
        {
            String* s = format(words[i]);
            FT(Vec(String), &v)->push_back(&v, *s);
            Call(String, s, Delete);
        }
        FT(Vec(String), &v)->erase(&v, 1);
        const String* a0 = FT(Vec(String), &v)->at(&v, 0);
        const String* a1 = FT(Vec(String), &v)->at(&v, 1);
        TEST_ASSERT(r, strcmp(a0->data, "aaa") == 0 && strcmp(a1->data, "ccc") == 0);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Foreach");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        for (int i = 0; i < 3; i++)
        {
            String* s = format("n{}", i);
            FT(Vec(String), &v)->push_back(&v, *s);
            Call(String, s, Delete);
        }
        int count = 0;
        foreach(Vec(String), str, v)
        {
            count++;
        }
        TEST_ASSERT(r, count == 3);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "EmplaceBack string");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        String* p = (String*)FT(Vec(String), &v)->emplace_back(&v);
        TEST_ASSERT(r, p != NULL && p->data != NULL);
        Call(String, p, Append, "in_place");
        TEST_ASSERT(r, strcmp(p->data, "in_place") == 0);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "SwapErase string");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        const char* words[] = { "aaa", "bbb", "ccc", "ddd" };
        for (int i = 0; i < 4; i++)
        {
            String* s = format(words[i]);
            FT(Vec(String), &v)->push_back(&v, *s);
            Call(String, s, Delete);
        }
        // [aaa, bbb, ccc, ddd] — swap_erase(1) → last (ddd) moves to 1 → [aaa, ddd, ccc]
        FT(Vec(String), &v)->swap_erase(&v, 1);
        const String* a0 = FT(Vec(String), &v)->at(&v, 0);
        const String* a1 = FT(Vec(String), &v)->at(&v, 1);
        const String* a2 = FT(Vec(String), &v)->at(&v, 2);
        TEST_ASSERT(r, strcmp(a0->data, "aaa") == 0 && strcmp(a1->data, "ddd") == 0);
        TEST_ASSERT(r, strcmp(a2->data, "ccc") == 0);
        v.vptr->destroy(&v);
    }
}

void vector_test_edge(TestRunner* r)
{
    TEST_GROUP(r, "Large reserve");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->reserve(&v, 100000);
        TEST_ASSERT(r, true);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Resize grow");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->resize(&v, 50);
        TEST_ASSERT(r, !VCall(Vec(f32), &v, is_empty));
        f32* a = FT(Vec(f32), &v)->at(&v, 49);
        TEST_ASSERT(r, *a == 0.0f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Resize shrink");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 0; i < 10; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        FT(Vec(f32), &v)->resize(&v, 3);
        f32* a = FT(Vec(f32), &v)->at(&v, 2);
        TEST_ASSERT(r, *a == 2.0f);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "100 create/destroy cycles");
    {
        for (int i = 0; i < 100; i++)
        {
            Vec(f32) v;
            Create(Vec(f32), &v);
            for (f32 j = 0; j < 10; j++)
                FT(Vec(f32), &v)->push_back(&v, j);
            v.vptr->destroy(&v);
        }
        TEST_ASSERT(r, true);
    }

    TEST_GROUP(r, "Integer types Vec(i32) Vec(u64)");
    {
        Vec(i32) vi;
        Create(Vec(i32), &vi);
        FT(Vec(i32), &vi)->push_back(&vi, -42);
        i32* a = FT(Vec(i32), &vi)->at(&vi, 0);
        TEST_ASSERT(r, *a == -42);
        vi.vptr->destroy(&vi);
    }
    {
        Vec(u64) vu;
        Create(Vec(u64), &vu);
        FT(Vec(u64), &vu)->push_back(&vu, 123456789ULL);
        u64* a = FT(Vec(u64), &vu)->at(&vu, 0);
        TEST_ASSERT(r, *a == 123456789ULL);
        vu.vptr->destroy(&vu);
    }

    TEST_GROUP(r, "Vec(Object)");
    {
        Vec(Object) vo;
        Create(Vec(Object), &vo);
        Object obj;
        Object_Create(&obj);
        FT(Vec(Object), &vo)->push_back(&vo, obj);
        TEST_ASSERT(r, !VCall(Vec(Object), &vo, is_empty));
        vo.vptr->destroy(&vo);
    }
}

void vector_test(void)
{
    TEST_INIT(runner, "Vector Tests");
    TEST_BEGIN(&runner);

    #ifdef VECTOR_TEST_POD
    vector_test_pod(&runner);
    #endif
    #ifdef VECTOR_TEST_STRING
    vector_test_string(&runner);
    #endif
    #ifdef VECTOR_TEST_EDGE
    vector_test_edge(&runner);
    #endif

    TEST_END(&runner);
}

#endif
