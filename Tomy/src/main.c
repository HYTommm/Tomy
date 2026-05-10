#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"

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

static int _test_npass = 0;
static int _test_nfail = 0;

static void _test_check(bool cond, const char* msg)
{
    print_emin(msg, set(sep = "", end = ""));
    if (cond)
    {
        println_emin(set_fg_idx(COLOR_BRIGHT_GREEN), "PASS", reset_style());
        _test_npass++;
    }
    else
    {
        println_emin(set_fg_idx(COLOR_BRIGHT_RED), "FAIL", reset_style());
        _test_nfail++;
    }
}

void vector_test_pod(void)
{
    println_emin("=== Vector POD (f32) Tests ===");

    // 1. Create / Destroy / IsEmpty
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        _test_check(VCall(Vec(f32), &v, is_empty), "Create/IsEmpty: ");
        v.vptr->destroy(&v);
    }

    // 2. PushBack / At
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->push_back(&v, 10.0f);
        FT(Vec(f32), &v)->push_back(&v, 20.0f);
        FT(Vec(f32), &v)->push_back(&v, 30.0f);
        _test_check(!VCall(Vec(f32), &v, is_empty), "PushBack: ");

        f32* a0 = FT(Vec(f32), &v)->at(&v, 0);
        f32* a1 = FT(Vec(f32), &v)->at(&v, 1);
        f32* a2 = FT(Vec(f32), &v)->at(&v, 2);
        _test_check(*a0 == 10.0f && *a1 == 20.0f && *a2 == 30.0f, "At access: ");

        v.vptr->destroy(&v);
    }

    // 3. Front / Back
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->push_back(&v, 1.0f);
        FT(Vec(f32), &v)->push_back(&v, 2.0f);
        FT(Vec(f32), &v)->push_back(&v, 3.0f);
        f32* f = FT(Vec(f32), &v)->front(&v);
        f32* b = FT(Vec(f32), &v)->back(&v);
        _test_check(*f == 1.0f && *b == 3.0f, "Front/Back: ");
        v.vptr->destroy(&v);
    }

    // 4. PopBack
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->push_back(&v, 1.0f);
        FT(Vec(f32), &v)->push_back(&v, 2.0f);
        FT(Vec(f32), &v)->push_back(&v, 3.0f);
        FT(Vec(f32), &v)->pop_back(&v);
        f32* b = FT(Vec(f32), &v)->back(&v);
        _test_check(*b == 2.0f, "PopBack: ");
        v.vptr->destroy(&v);
    }

    // 5. Erase
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 1; i <= 5; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        FT(Vec(f32), &v)->erase(&v, 1);
        f32* a0 = FT(Vec(f32), &v)->at(&v, 0);
        f32* a1 = FT(Vec(f32), &v)->at(&v, 1);
        _test_check(*a0 == 1.0f && *a1 == 3.0f, "Erase middle: ");

        FT(Vec(f32), &v)->erase(&v, 3);
        f32* b = FT(Vec(f32), &v)->back(&v);
        _test_check(*b == 4.0f, "Erase last: ");

        FT(Vec(f32), &v)->erase(&v, 0);
        f32* f = FT(Vec(f32), &v)->front(&v);
        _test_check(*f == 3.0f, "Erase first: ");
        v.vptr->destroy(&v);
    }

    // 6. Reserve
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->reserve(&v, 1000);
        _test_check(true, "Reserve: ");
        v.vptr->destroy(&v);
    }

    // 7. Clear / Re-push
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 0; i < 10; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        FT(Vec(f32), &v)->clear(&v);
        _test_check(VCall(Vec(f32), &v, is_empty), "Clear: ");

        FT(Vec(f32), &v)->push_back(&v, 42.0f);
        f32* a = FT(Vec(f32), &v)->at(&v, 0);
        _test_check(*a == 42.0f, "Push after clear: ");
        v.vptr->destroy(&v);
    }

    // 8. Foreach iteration
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
        _test_check(sum == 10.0f, "Foreach sum: ");
        v.vptr->destroy(&v);
    }

    // 9. Iterator
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->push_back(&v, 100.0f);
        FT(Vec(f32), &v)->push_back(&v, 200.0f);

        VecIter(f32) it = VCall(Vec(f32), &v, begin);
        VecIter(f32) end_it = VCall(Vec(f32), &v, end);
        _test_check(!VCall(VecIter(f32), &it, equals, &end_it), "Iterator begin != end: ");

        f32 val = VCall(VecIter(f32), &it, get);
        _test_check(val == 100.0f, "Iterator get 1st: ");

        VCall(VecIter(f32), &it, next);
        val = VCall(VecIter(f32), &it, get);
        _test_check(val == 200.0f, "Iterator get 2nd: ");

        VCall(VecIter(f32), &it, next);
        _test_check(VCall(VecIter(f32), &it, equals, &end_it), "Iterator at end: ");
        v.vptr->destroy(&v);
    }

    // 10. Large scale
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
        _test_check(ok, "10000 elements: ");

        f32* data = FT(Vec(f32), &v)->data(&v);
        _test_check(data[0] == 0.0f && data[9999] == 9999.0f, "Data pointer: ");
        v.vptr->destroy(&v);
    }

    // 11. EmplaceBack POD
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        f32* p = (f32*)FT(Vec(f32), &v)->emplace_back(&v);
        _test_check(p != NULL && !VCall(Vec(f32), &v, is_empty), "EmplaceBack POD: ");
        *p = 3.14f;
        f32* b = FT(Vec(f32), &v)->back(&v);
        _test_check(*b == 3.14f, "EmplaceBack value: ");
        v.vptr->destroy(&v);
    }

    // 12. SwapErase POD
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 0; i < 4; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        // [0, 1, 2, 3] — swap_erase(1) → last element (3) moves to 1 → [0, 3, 2]
        FT(Vec(f32), &v)->swap_erase(&v, 1);
        f32* a0 = FT(Vec(f32), &v)->at(&v, 0);
        f32* a1 = FT(Vec(f32), &v)->at(&v, 1);
        _test_check(*a0 == 0.0f && *a1 == 3.0f, "SwapErase middle: ");
        f32* b = FT(Vec(f32), &v)->back(&v);
        _test_check(*b == 2.0f, "SwapErase back: ");
        v.vptr->destroy(&v);
    }
}

void vector_test_string(void)
{
    println_emin("=== Vector String Tests ===");

    // 1. Create / Destroy
    {
        Vec(String) v;
        Create(Vec(String), &v);
        _test_check(VCall(Vec(String), &v, is_empty), "Create empty: ");
        v.vptr->destroy(&v);
    }

    // 2. PushBack / At
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
        _test_check(strcmp(a0->data, "hello") == 0, "String[0]: ");
        _test_check(strcmp(a1->data, "world") == 0, "String[1]: ");
        _test_check(strcmp(a2->data, "test") == 0, "String[2]: ");
        v.vptr->destroy(&v);
    }

    // 3. Clear
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
        _test_check(VCall(Vec(String), &v, is_empty), "Clear strings: ");

        String* s = format("after_clear");
        FT(Vec(String), &v)->push_back(&v, *s);
        Call(String, s, Delete);
        const String* a = FT(Vec(String), &v)->at(&v, 0);
        _test_check(strcmp(a->data, "after_clear") == 0, "Re-push string: ");
        v.vptr->destroy(&v);
    }

    // 4. Erase string
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
        _test_check(strcmp(a0->data, "aaa") == 0 && strcmp(a1->data, "ccc") == 0, "Erase string: ");
        v.vptr->destroy(&v);
    }

    // 5. Foreach
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
        _test_check(count == 3, "Foreach strings: ");
        v.vptr->destroy(&v);
    }

    // 6. EmplaceBack string (in-place construct)
    {
        Vec(String) v;
        Create(Vec(String), &v);
        String* p = (String*)FT(Vec(String), &v)->emplace_back(&v);
        _test_check(p != NULL && p->data != NULL, "EmplaceBack string: ");
        Call(String, p, Append, "in_place");
        _test_check(strcmp(p->data, "in_place") == 0, "EmplaceBack assign: ");
        v.vptr->destroy(&v);
    }

    // 7. SwapErase string
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
        _test_check(strcmp(a0->data, "aaa") == 0 && strcmp(a1->data, "ddd") == 0, "SwapErase string: ");
        _test_check(strcmp(a2->data, "ccc") == 0, "SwapErase unchanged: ");
        v.vptr->destroy(&v);
    }
}

void vector_test_edge(void)
{
    println_emin("=== Vector Edge Case Tests ===");

    // 1. Large reserve
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->reserve(&v, 100000);
        _test_check(true, "Large reserve: ");
        v.vptr->destroy(&v);
    }

    // 2. Resize grow
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        FT(Vec(f32), &v)->resize(&v, 50);
        _test_check(!VCall(Vec(f32), &v, is_empty), "Resize grow: ");
        f32* a = FT(Vec(f32), &v)->at(&v, 49);
        _test_check(*a == 0.0f, "Resize zero-init: ");
        v.vptr->destroy(&v);
    }

    // 3. Resize shrink
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        for (f32 i = 0; i < 10; i++)
            FT(Vec(f32), &v)->push_back(&v, i);
        FT(Vec(f32), &v)->resize(&v, 3);
        f32* a = FT(Vec(f32), &v)->at(&v, 2);
        _test_check(*a == 2.0f, "Resize shrink: ");
        v.vptr->destroy(&v);
    }

    // 4. Multiple create/destroy
    {
        for (int i = 0; i < 100; i++)
        {
            Vec(f32) v;
            Create(Vec(f32), &v);
            for (f32 j = 0; j < 10; j++)
                FT(Vec(f32), &v)->push_back(&v, j);
            v.vptr->destroy(&v);
        }
        _test_check(true, "100 cycles: ");
    }

    // 5. Integer types
    {
        Vec(i32) vi;
        Create(Vec(i32), &vi);
        FT(Vec(i32), &vi)->push_back(&vi, -42);
        i32* a = FT(Vec(i32), &vi)->at(&vi, 0);
        _test_check(*a == -42, "Vec(i32): ");
        vi.vptr->destroy(&vi);
    }
    {
        Vec(u64) vu;
        Create(Vec(u64), &vu);
        FT(Vec(u64), &vu)->push_back(&vu, 123456789ULL);
        u64* a = FT(Vec(u64), &vu)->at(&vu, 0);
        _test_check(*a == 123456789ULL, "Vec(u64): ");
        vu.vptr->destroy(&vu);
    }

    // 6. Vec(Object)
    {
        Vec(Object) vo;
        Create(Vec(Object), &vo);
        Object obj;
        Object_Create(&obj);
        FT(Vec(Object), &vo)->push_back(&vo, obj);
        _test_check(!VCall(Vec(Object), &vo, is_empty), "Vec(Object): ");
        vo.vptr->destroy(&vo);
    }
}

void vector_test(void)
{
    _test_npass = 0;
    _test_nfail = 0;

    #ifdef VECTOR_TEST_POD
    vector_test_pod();
    #endif
    #ifdef VECTOR_TEST_STRING
    vector_test_string();
    #endif
    #ifdef VECTOR_TEST_EDGE
    vector_test_edge();
    #endif

    println_emin();
    println_emin("Results:");
    println_emin(set_fg_idx(COLOR_BRIGHT_GREEN), "PASS ", _test_npass,
        set_fg_idx(COLOR_BRIGHT_RED), "FAIL", _test_nfail, reset_style());
    println_emin("Total:", _test_npass + _test_nfail);
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
    print_color_test();
    #endif
    #ifdef PRINT_256_COLOR_TABLE
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
}

int main(int argc, char** argv)
{
    test();
    return 0;
}