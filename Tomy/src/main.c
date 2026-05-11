#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"

//#define PRINT_ANY_TEST
//#define PRINT_FMT_TEST
//#define PRINT_COLOR_TEST
//#define PRINT_256_COLOR_TABLE
//#define TEST_1
//#define TEST_2
//#define TEST_3

#define VECTOR_TEST
#ifdef VECTOR_TEST
#define VECTOR_TEST_POD
#define VECTOR_TEST_STRING
#define VECTOR_TEST_EDGE
#endif

#define HASHMAP_TEST
#ifdef HASHMAP_TEST
#define HASHMAP_TEST_POD
#define HASHMAP_TEST_STRING
#define HASHMAP_TEST_EDGE
#endif

#define LIST_TEST
#ifdef LIST_TEST
#define LIST_TEST_POD
#define LIST_TEST_STRING
#define LIST_TEST_EDGE
#endif

#define DLIST_TEST
#ifdef DLIST_TEST
#define DLIST_TEST_POD
#define DLIST_TEST_STRING
#define DLIST_TEST_EDGE
#endif

void print_256_color_table(void)
{
    for (int i = 0; i < 256; i++)
    {
        char buf[4] = { 0 };
        snprintf(buf, sizeof(buf), "%3d", i);
        print_emin(set_bg_idx(i), buf, reset_style(), set(sep = "", end = ""));
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }
    }
}

void print_any_test(void)
{
    print_emin();
    print_emin("Hello", "Emin", 42, 3.141592);
}

void print_fmt_test(void)
{
    print_emin("{} is {}", "Emin", 42);
}

void print_color_test(void)
{
    print_emin(set_colors_idx(COLOR_RED, COLOR_WHITE), "REDRUM", reset_style(), set(sep = ""));
    print_emin(set_fg_rgb(102, 255, 178), "Here is Johnny~", reset_style(), set(sep = ""));
}

void test_1(void)
{
    print_emin(1, 2, set(sep = "", end = ""));
    print_emin(3, 4, set(sep = "", end = ""));
    print_emin(set_cursor_pos(2, 1), set(end = ""));
    print_emin(set_fg_idx(COLOR_BRIGHT_GREEN), format("{}/{}", 70, 100), reset_style(), set(sep = ""));
}

void test_2(void)
{
    String* content = format("{{+} print in {}", "C23");
    print_emin((const String*)content);
    FILE* file = fopen("test.txt", "wb");
    print_emin(content, set(file = file));
    fclose(file);
}

void test_3(void)
{
    Color24 color = rgba(255, 0, 0, 1);
    print_emin(color);
    color = rgba(0, 255, 0, 1);
    print_emin(&color);
    color = rgba(144, 12, 12, 1);
    print_emin(set_fg_color(color), "MURDER", reset_style(), set(sep = ""));
}

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

#ifdef HASHMAP_TEST

void hashmap_test_pod(TestRunner* r)
{
    TEST_GROUP(r, "Create / Size / Empty");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, IsEmpty));
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Insert / Find");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 10.5);
        Call(HashMap(i32, f64), &m, Insert, 2, 20.5);
        Call(HashMap(i32, f64), &m, Insert, 3, 30.5);

        f64* v1 = Call(HashMap(i32, f64), &m, At, 1);
        f64* v2 = Call(HashMap(i32, f64), &m, At, 2);
        f64* v3 = Call(HashMap(i32, f64), &m, At, 3);
        TEST_ASSERT(r, v1 && *v1 == 10.5 && v2 && *v2 == 20.5 && v3 && *v3 == 30.5);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 3);

        f64* vn = Call(HashMap(i32, f64), &m, At, 99);
        TEST_ASSERT(r, vn == NULL);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Insert overwrite");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 100.0);
        Call(HashMap(i32, f64), &m, Insert, 1, 200.0);
        f64* v = Call(HashMap(i32, f64), &m, At, 1);
        TEST_ASSERT(r, v && *v == 200.0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 1);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Contains");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 5, 50.0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Contains, 5));
        TEST_ASSERT(r, !Call(HashMap(i32, f64), &m, Contains, 99));
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Erase");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 10.0);
        Call(HashMap(i32, f64), &m, Insert, 2, 20.0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Erase, 1));
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, At, 1) == NULL);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, At, 2) != NULL);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 1);

        TEST_ASSERT(r, !Call(HashMap(i32, f64), &m, Erase, 99));
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Clear / Re-insert");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 1.0);
        Call(HashMap(i32, f64), &m, Insert, 2, 2.0);
        Call(HashMap(i32, f64), &m, Insert, 3, 3.0);
        Call(HashMap(i32, f64), &m, Clear);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, At, 1) == NULL);

        Call(HashMap(i32, f64), &m, Insert, 42, 42.0);
        f64* v = Call(HashMap(i32, f64), &m, At, 42);
        TEST_ASSERT(r, v && *v == 42.0);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Tombstone reuse");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 1.0);
        Call(HashMap(i32, f64), &m, Insert, 2, 2.0);
        Call(HashMap(i32, f64), &m, Erase, 1);
        Call(HashMap(i32, f64), &m, Insert, 3, 3.0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 2);
        f64* v2 = Call(HashMap(i32, f64), &m, At, 2);
        f64* v3 = Call(HashMap(i32, f64), &m, At, 3);
        TEST_ASSERT(r, v2 && *v2 == 2.0 && v3 && *v3 == 3.0);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Rehash (1000 inserts)");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        for (i32 i = 0; i < 1000; i++)
            Call(HashMap(i32, f64), &m, Insert, i, (f64)i * 1.5);

        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 1000);

        bool ok = true;
        for (i32 i = 0; i < 1000; i++)
        {
            f64* v = Call(HashMap(i32, f64), &m, At, i);
            if (!v || *v != (f64)i * 1.5)
            {
                ok = false; break;
            }
        }
        TEST_ASSERT(r, ok);

        for (i32 i = 0; i < 500; i++)
            Call(HashMap(i32, f64), &m, Erase, i);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 500);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Independent maps");
    {
        HashMap(i32, f64) a, b;
        Create(HashMap(i32, f64), &a);
        Create(HashMap(i32, f64), &b);
        Call(HashMap(i32, f64), &a, Insert, 1, 100.0);
        Call(HashMap(i32, f64), &b, Insert, 1, 200.0);
        f64* va = Call(HashMap(i32, f64), &a, At, 1);
        f64* vb = Call(HashMap(i32, f64), &b, At, 1);
        TEST_ASSERT(r, va && *va == 100.0 && vb && *vb == 200.0);
        a.vptr->destroy(&a);
        b.vptr->destroy(&b);
    }

    TEST_GROUP(r, "Iterator basic");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 10.0);
        Call(HashMap(i32, f64), &m, Insert, 2, 20.0);
        Call(HashMap(i32, f64), &m, Insert, 3, 30.0);

        HashMapIter(i32, f64) it = VCall(HashMap(i32, f64), &m, begin);
        HashMapIter(i32, f64) end = VCall(HashMap(i32, f64), &m, end);

        bool ok = true;
        f64 sum = 0;
        int count = 0;
        while (!VCall(HashMapIter(i32, f64), &it, equals, &end))
        {
            sum += *VCall(HashMapIter(i32, f64), &it, get).value;
            count++;
            VCall(HashMapIter(i32, f64), &it, next);
        }
        TEST_ASSERT(r, count == 3 && sum == 60.0);

        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Iterator key/value access");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 42, 3.14);
        Call(HashMap(i32, f64), &m, Insert, 99, 2.71);

        HashMapIter(i32, f64) it = VCall(HashMap(i32, f64), &m, begin);
        HashMapIter(i32, f64) end = VCall(HashMap(i32, f64), &m, end);

        int found_keys[2] = { 0 };
        while (!VCall(HashMapIter(i32, f64), &it, equals, &end))
        {
            i32* k = VCall(HashMapIter(i32, f64), &it, key);
            f64* v = VCall(HashMapIter(i32, f64), &it, value);
            if (*k == 42)
            {
                TEST_ASSERT(r, *v == 3.14); found_keys[0] = 1;
            }
            if (*k == 99)
            {
                TEST_ASSERT(r, *v == 2.71); found_keys[1] = 1;
            }
            VCall(HashMapIter(i32, f64), &it, next);
        }
        TEST_ASSERT(r, found_keys[0] && found_keys[1]);

        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Foreach Pair");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 10.0);
        Call(HashMap(i32, f64), &m, Insert, 2, 20.0);
        Call(HashMap(i32, f64), &m, Insert, 3, 30.0);

        f64 sum = 0;
        int count = 0;
        foreach(HashMap(i32, f64), p, m)
        {
            sum += *p.value;
            count++;
        }
        TEST_ASSERT(r, count == 3 && sum == 60.0);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "TryEmplace new key");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        i32 k = 42; f64 v = 3.14;
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, TryEmplace, &k, &v));
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 1);
        f64* found = Call(HashMap(i32, f64), &m, At, 42);
        TEST_ASSERT(r, found && *found == 3.14);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "TryEmplace existing key");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 100.0);
        i32 k = 1; f64 v = 999.0;
        TEST_ASSERT(r, !Call(HashMap(i32, f64), &m, TryEmplace, &k, &v));
        f64* found = Call(HashMap(i32, f64), &m, At, 1);
        TEST_ASSERT(r, found && *found == 100.0);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "TryEmplace rehash (1000)");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        for (i32 i = 0; i < 1000; i++)
        {
            f64 val = (f64)i;
            Call(HashMap(i32, f64), &m, TryEmplace, &i, &val);
        }
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 1000);
        bool ok = true;
        for (i32 i = 0; i < 1000; i++)
        {
            f64* fv = Call(HashMap(i32, f64), &m, At, i);
            if (!fv || *fv != (f64)i)
            {
                ok = false; break;
            }
        }
        TEST_ASSERT(r, ok);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Find returns iterator");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 10.0);
        Call(HashMap(i32, f64), &m, Insert, 2, 20.0);
        Call(HashMap(i32, f64), &m, Insert, 3, 30.0);

        HashMapIter(i32, f64) end = VCall(HashMap(i32, f64), &m, end);

        HashMapIter(i32, f64) it = Call(HashMap(i32, f64), &m, Find, 2);
        TEST_ASSERT(r, !VCall(HashMapIter(i32, f64), &it, equals, &end));
        TEST_ASSERT(r, *VCall(HashMapIter(i32, f64), &it, get).value == 20.0);
        TEST_ASSERT(r, *VCall(HashMapIter(i32, f64), &it, key) == 2);

        it = Call(HashMap(i32, f64), &m, Find, 99);
        TEST_ASSERT(r, VCall(HashMapIter(i32, f64), &it, equals, &end));
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Count");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Count, 1) == 0);
        Call(HashMap(i32, f64), &m, Insert, 1, 1.0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Count, 1) == 1);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Count, 2) == 0);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Rehash explicit");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        for (i32 i = 0; i < 100; i++)
            Call(HashMap(i32, f64), &m, Insert, i, (f64)i);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 100);
        Call(HashMap(i32, f64), &m, Rehash, 200);
        bool ok = true;
        for (i32 i = 0; i < 100; i++)
        {
            f64* v = Call(HashMap(i32, f64), &m, At, i);
            if (!v || *v != (f64)i)
            {
                ok = false; break;
            }
        }
        TEST_ASSERT(r, ok);
        m.vptr->destroy(&m);
    }
}

void hashmap_test_string(TestRunner* r)
{
    TEST_GROUP(r, "Insert / Find String key");
    {
        HashMap(String, i32) m;
        Create(HashMap(String, i32), &m);

        String* k1 = New(String, 16); Call(String, k1, Append, "hello");
        String* k2 = New(String, 16); Call(String, k2, Append, "world");

        Call(HashMap(String, i32), &m, Insert, *k1, 100);
        Call(HashMap(String, i32), &m, Insert, *k2, 200);

        i32* v1 = Call(HashMap(String, i32), &m, At, *k1);
        i32* v2 = Call(HashMap(String, i32), &m, At, *k2);
        TEST_ASSERT(r, v1 && *v1 == 100);
        TEST_ASSERT(r, v2 && *v2 == 200);
        TEST_ASSERT(r, Call(HashMap(String, i32), &m, Size) == 2);

        Call(String, k1, Delete);
        Call(String, k2, Delete);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Same content match");
    {
        HashMap(String, i32) m;
        Create(HashMap(String, i32), &m);

        String* a = New(String, 16); Call(String, a, Append, "key");
        String* b = New(String, 16); Call(String, b, Append, "key");

        Call(HashMap(String, i32), &m, Insert, *a, 42);
        i32* v = Call(HashMap(String, i32), &m, At, *b);
        TEST_ASSERT(r, v && *v == 42);

        Call(String, a, Delete);
        Call(String, b, Delete);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Clear string keys");
    {
        HashMap(String, i32) m;
        Create(HashMap(String, i32), &m);
        for (int i = 0; i < 10; i++)
        {
            String* k = New(String, 16);
            char buf[32]; snprintf(buf, sizeof(buf), "k%d", i);
            Call(String, k, Append, buf);
            Call(HashMap(String, i32), &m, Insert, *k, i);
            Call(String, k, Delete);
        }
        TEST_ASSERT(r, Call(HashMap(String, i32), &m, Size) == 10);
        Call(HashMap(String, i32), &m, Clear);
        TEST_ASSERT(r, Call(HashMap(String, i32), &m, Size) == 0);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "TryEmplace String key");
    {
        HashMap(String, i32) m;
        Create(HashMap(String, i32), &m);

        String* k = New(String, 16); Call(String, k, Append, "hello");
        i32 v = 42;
        TEST_ASSERT(r, Call(HashMap(String, i32), &m, TryEmplace, k, &v));
        TEST_ASSERT(r, Call(HashMap(String, i32), &m, Size) == 1);
        i32* found = Call(HashMap(String, i32), &m, At, *k);
        TEST_ASSERT(r, found && *found == 42);
        Call(String, k, Delete);

        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "TryEmplace existing String");
    {
        HashMap(String, i32) m;
        Create(HashMap(String, i32), &m);

        String* k = New(String, 16); Call(String, k, Append, "key");
        String* k2 = New(String, 16); Call(String, k2, Append, "key");
        Call(HashMap(String, i32), &m, Insert, *k, 100);
        i32 v = 999;
        TEST_ASSERT(r, !Call(HashMap(String, i32), &m, TryEmplace, k2, &v));
        i32* found = Call(HashMap(String, i32), &m, At, *k);
        TEST_ASSERT(r, found && *found == 100);
        Call(String, k, Delete);
        Call(String, k2, Delete);

        m.vptr->destroy(&m);
    }
}

void hashmap_test_edge(TestRunner* r)
{
    TEST_GROUP(r, "Empty map ops (no crash)");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, At, 1) == NULL);
        TEST_ASSERT(r, !Call(HashMap(i32, f64), &m, Erase, 1));
        TEST_ASSERT(r, !Call(HashMap(i32, f64), &m, Contains, 1));
        Call(HashMap(i32, f64), &m, Clear);
        TEST_ASSERT(r, true);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "100x overwrite same key");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        for (i32 i = 0; i < 100; i++)
            Call(HashMap(i32, f64), &m, Insert, 42, (f64)i);
        f64* v = Call(HashMap(i32, f64), &m, At, 42);
        TEST_ASSERT(r, v && *v == 99.0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 1);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "50 create/destroy cycles");
    {
        for (int i = 0; i < 50; i++)
        {
            HashMap(i32, f64) m;
            Create(HashMap(i32, f64), &m);
            for (i32 j = 0; j < 10; j++)
                Call(HashMap(i32, f64), &m, Insert, j, (f64)j);
            m.vptr->destroy(&m);
        }
        TEST_ASSERT(r, true);
    }

    TEST_GROUP(r, "ShrinkToFit after erase");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        for (i32 i = 0; i < 100; i++)
            Call(HashMap(i32, f64), &m, Insert, i, (f64)i);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 100);

        for (i32 i = 10; i < 100; i++)
            Call(HashMap(i32, f64), &m, Erase, i);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 10);

        Call(HashMap(i32, f64), &m, ShrinkToFit);
        bool ok = true;
        for (i32 i = 0; i < 10; i++)
        {
            f64* v = Call(HashMap(i32, f64), &m, At, i);
            if (!v || *v != (f64)i)
            {
                ok = false; break;
            }
        }
        TEST_ASSERT(r, ok);

        Call(HashMap(i32, f64), &m, Insert, 999, 999.0);
        f64* v = Call(HashMap(i32, f64), &m, At, 999);
        TEST_ASSERT(r, v && *v == 999.0);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "ShrinkToFit empty / single");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, ShrinkToFit);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 0);
        m.vptr->destroy(&m);
    }
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 1, 1.0);
        Call(HashMap(i32, f64), &m, Erase, 1);
        Call(HashMap(i32, f64), &m, ShrinkToFit);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, Size) == 0);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Empty iterator");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        HashMapIter(i32, f64) it = VCall(HashMap(i32, f64), &m, begin);
        HashMapIter(i32, f64) end = VCall(HashMap(i32, f64), &m, end);
        TEST_ASSERT(r, VCall(HashMapIter(i32, f64), &it, equals, &end));
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Single element iterator");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        Call(HashMap(i32, f64), &m, Insert, 7, 7.7);
        HashMapIter(i32, f64) it = VCall(HashMap(i32, f64), &m, begin);
        HashMapIter(i32, f64) end = VCall(HashMap(i32, f64), &m, end);
        TEST_ASSERT(r, !VCall(HashMapIter(i32, f64), &it, equals, &end));
        f64 val = *VCall(HashMapIter(i32, f64), &it, get).value;
        TEST_ASSERT(r, val == 7.7);
        VCall(HashMapIter(i32, f64), &it, next);
        TEST_ASSERT(r, VCall(HashMapIter(i32, f64), &it, equals, &end));
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "IsEmpty lifecycle");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, IsEmpty));
        Call(HashMap(i32, f64), &m, Insert, 1, 1.0);
        TEST_ASSERT(r, !Call(HashMap(i32, f64), &m, IsEmpty));
        Call(HashMap(i32, f64), &m, Clear);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, IsEmpty));
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "LoadFactor");
    {
        HashMap(i32, f64) m;
        Create(HashMap(i32, f64), &m);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &m, LoadFactor) == 0.0);
        Call(HashMap(i32, f64), &m, Insert, 1, 1.0);
        Call(HashMap(i32, f64), &m, Insert, 2, 2.0);
        Call(HashMap(i32, f64), &m, Insert, 3, 3.0);
        double lf = Call(HashMap(i32, f64), &m, LoadFactor);
        TEST_ASSERT(r, lf > 0.0 && lf <= 0.75);
        m.vptr->destroy(&m);
    }

    TEST_GROUP(r, "Swap");
    {
        HashMap(i32, f64) a, b;
        Create(HashMap(i32, f64), &a);
        Create(HashMap(i32, f64), &b);
        Call(HashMap(i32, f64), &a, Insert, 1, 100.0);
        Call(HashMap(i32, f64), &a, Insert, 2, 200.0);
        Call(HashMap(i32, f64), &b, Insert, 3, 300.0);
        Call(HashMap(i32, f64), &a, Swap, &b);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &a, At, 3) != NULL);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &b, At, 1) != NULL);
        f64* v = Call(HashMap(i32, f64), &a, At, 3);
        TEST_ASSERT(r, v && *v == 300.0);
        v = Call(HashMap(i32, f64), &b, At, 2);
        TEST_ASSERT(r, v && *v == 200.0);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &a, Size) == 1);
        TEST_ASSERT(r, Call(HashMap(i32, f64), &b, Size) == 2);
        a.vptr->destroy(&a);
        b.vptr->destroy(&b);
    }
}

void hashmap_test(void)
{
    TEST_INIT(runner, "HashMap Tests");
    TEST_BEGIN(&runner);

    #ifdef HASHMAP_TEST_POD
    hashmap_test_pod(&runner);
    #endif
    #ifdef HASHMAP_TEST_STRING
    hashmap_test_string(&runner);
    #endif
    #ifdef HASHMAP_TEST_EDGE
    hashmap_test_edge(&runner);
    #endif

    TEST_END(&runner);
}

#endif

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
            if (val != idx) { ok = false; break; }
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

        i32 vals[3] = {0};
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

        i32 vals[3] = {0};
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
            if (v != idx) { ok = false; break; }
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

void test(void)
{
    #ifdef PRINT_ANY_TEST
    print_any_test();
    #endif
    #ifdef PRINT_FMT_TEST
    print_fmt_test();
    #endif
    #ifdef PRINT_COLOR_TEST
    print_256_color_table();
    #endif
    #ifdef TEST_1
    test_1();
    #endif
    #ifdef TEST_2
    test_2();
    #endif
    #ifdef TEST_3
    test_3();
    #endif
    #ifdef VECTOR_TEST
    vector_test();
    #endif
    #ifdef HASHMAP_TEST
    hashmap_test();
    #endif
    #ifdef LIST_TEST
    list_test();
    #endif
    #ifdef DLIST_TEST
    dlist_test();
    #endif
}

int main(int argc, char** argv)
{
    test();
    return 0;
}