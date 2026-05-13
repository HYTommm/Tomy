#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef ALGORITHM_TEST

// ============================================================
// 自定义比较器（仅保留非默认逻辑）
// ============================================================

static int _cmp_i32_desc(const i32* a, const i32* b)
{
    return (*a < *b) - (*a > *b);
}

// ============================================================
// 辅助：验证 Vector 升序
// ============================================================

static bool _vec_sorted_asc_i32(const Vec(i32)* v)
{
    for (umax i = 1; i < v->size; i++)
    {
        i32* prev = FT(Vec(i32), v)->at(v, i - 1);
        i32* cur = FT(Vec(i32), v)->at(v, i);
        if (*prev > *cur) return false;
    }
    return true;
}

static bool _vec_sorted_asc_f64(const Vec(f64)* v)
{
    for (umax i = 1; i < v->size; i++)
    {
        f64* prev = FT(Vec(f64), v)->at(v, i - 1);
        f64* cur = FT(Vec(f64), v)->at(v, i);
        if (*prev > *cur) return false;
    }
    return true;
}

static bool _vec_sorted_asc_String(const Vec(String)* v)
{
    for (umax i = 1; i < v->size; i++)
    {
        const String* prev = FT(Vec(String), v)->at(v, i - 1);
        const String* cur = FT(Vec(String), v)->at(v, i);
        if (strcmp(prev->data, cur->data) > 0) return false;
    }
    return true;
}

// ============================================================
// 辅助：验证 List 升序
// ============================================================

static bool _list_check_asc_i32(const List(i32)* l)
{
    if (l->size <= 1) return true;
    ListIter(i32) it = VCall(List(i32), l, begin);
    ListIter(i32) end = VCall(List(i32), l, end);
    i32 prev = VCall(ListIter(i32), &it, get);
    VCall(ListIter(i32), &it, next);
    while (!VCall(ListIter(i32), &it, equals, &end))
    {
        i32 cur = VCall(ListIter(i32), &it, get);
        if (prev > cur) return false;
        prev = cur;
        VCall(ListIter(i32), &it, next);
    }
    return true;
}

static bool _list_check_asc_String(const List(String)* l)
{
    if (l->size <= 1) return true;
    ListIter(String) it = VCall(List(String), l, begin);
    ListIter(String) end = VCall(List(String), l, end);
    String prev = VCall(ListIter(String), &it, get);
    VCall(ListIter(String), &it, next);
    while (!VCall(ListIter(String), &it, equals, &end))
    {
        String cur = VCall(ListIter(String), &it, get);
        if (strcmp(prev.data, cur.data) > 0) return false;
        prev = cur;
        VCall(ListIter(String), &it, next);
    }
    return true;
}

// ============================================================
// Vector Sort Tests (POD)
// ============================================================

static void _test_vector_sort_pod(TestRunner* r)
{
    TEST_GROUP(r, "Vec(f64) ascending");
    {
        Vec(f64) v;
        Create(Vec(f64), &v);
        FT(Vec(f64), &v)->push_back(&v, 3.0);
        FT(Vec(f64), &v)->push_back(&v, 1.0);
        FT(Vec(f64), &v)->push_back(&v, 2.0);
        SORT_CMP(Vec(f64), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_f64(&v));
        TEST_ASSERT(r, *FT(Vec(f64), &v)->at(&v, 0) == 1.0);
        TEST_ASSERT(r, *FT(Vec(f64), &v)->at(&v, 2) == 3.0);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(i32) descending cmp");
    {
        Vec(i32) v;
        Create(Vec(i32), &v);
        FT(Vec(i32), &v)->push_back(&v, 1);
        FT(Vec(i32), &v)->push_back(&v, 3);
        FT(Vec(i32), &v)->push_back(&v, 2);
        SORT_CMP(Vec(i32), &v, _cmp_i32_desc);
        TEST_ASSERT(r, *FT(Vec(i32), &v)->at(&v, 0) == 3);
        TEST_ASSERT(r, *FT(Vec(i32), &v)->at(&v, 2) == 1);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(f32) empty (no crash)");
    {
        Vec(f32) v;
        Create(Vec(f32), &v);
        SORT_CMP(Vec(f32), &v, NULL);
        TEST_ASSERT(r, VCall(Vec(f32), &v, is_empty));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(i32) single element");
    {
        Vec(i32) v;
        Create(Vec(i32), &v);
        FT(Vec(i32), &v)->push_back(&v, 42);
        SORT_CMP(Vec(i32), &v, NULL);
        TEST_ASSERT(r, *FT(Vec(i32), &v)->at(&v, 0) == 42);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(f64) already sorted");
    {
        Vec(f64) v;
        Create(Vec(f64), &v);
        for (f64 i = 0; i < 10; i++) FT(Vec(f64), &v)->push_back(&v, i);
        SORT_CMP(Vec(f64), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_f64(&v));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(i32) reverse sorted");
    {
        Vec(i32) v;
        Create(Vec(i32), &v);
        for (i32 i = 9; i >= 0; i--) FT(Vec(i32), &v)->push_back(&v, i);
        SORT_CMP(Vec(i32), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_i32(&v));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(f64) duplicates");
    {
        Vec(f64) v;
        Create(Vec(f64), &v);
        FT(Vec(f64), &v)->push_back(&v, 5.0);
        FT(Vec(f64), &v)->push_back(&v, 1.0);
        FT(Vec(f64), &v)->push_back(&v, 3.0);
        FT(Vec(f64), &v)->push_back(&v, 1.0);
        FT(Vec(f64), &v)->push_back(&v, 5.0);
        SORT_CMP(Vec(f64), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_f64(&v));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(i32) large scale (2000)");
    {
        Vec(i32) v;
        Create(Vec(i32), &v);
        for (i32 i = 1999; i >= 0; i--) FT(Vec(i32), &v)->push_back(&v, i);
        SORT_CMP(Vec(i32), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_i32(&v));
        v.vptr->destroy(&v);
    }
}

// ============================================================
// Vector Sort Tests (String)
// ============================================================

static void _test_vector_sort_string(TestRunner* r)
{
    TEST_GROUP(r, "Vec(String) ascending");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        const char* words[] = { "banana", "apple", "cherry", "Apple" };
        for (int i = 0; i < 4; i++)
        {
            String* s = format(words[i]);
            FT(Vec(String), &v)->push_back(&v, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(Vec(String), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_String(&v));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(String) empty (no crash)");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        SORT_CMP(Vec(String), &v, NULL);
        TEST_ASSERT(r, VCall(Vec(String), &v, is_empty));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(String) single element");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        String* s = format("only");
        FT(Vec(String), &v)->push_back(&v, *s);
        Call(String, s, Delete);
        SORT_CMP(Vec(String), &v, NULL);
        const String* a = FT(Vec(String), &v)->at(&v, 0);
        TEST_ASSERT(r, strcmp(a->data, "only") == 0);
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(String) already sorted");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        const char* words[] = { "cat", "dog", "elephant" };
        for (int i = 0; i < 3; i++)
        {
            String* s = format(words[i]);
            FT(Vec(String), &v)->push_back(&v, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(Vec(String), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_String(&v));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(String) reverse sorted");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        const char* words[] = { "zzz", "mmm", "aaa" };
        for (int i = 0; i < 3; i++)
        {
            String* s = format(words[i]);
            FT(Vec(String), &v)->push_back(&v, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(Vec(String), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_String(&v));
        v.vptr->destroy(&v);
    }

    TEST_GROUP(r, "Vec(String) duplicates");
    {
        Vec(String) v;
        Create(Vec(String), &v);
        const char* words[] = { "x", "a", "x", "b", "a" };
        for (int i = 0; i < 5; i++)
        {
            String* s = format(words[i]);
            FT(Vec(String), &v)->push_back(&v, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(Vec(String), &v, NULL);
        TEST_ASSERT(r, _vec_sorted_asc_String(&v));
        v.vptr->destroy(&v);
    }
}

// ============================================================
// List Sort Tests (POD)
// ============================================================

static void _test_list_sort_pod(TestRunner* r)
{
    TEST_GROUP(r, "List(i32) ascending");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 3);
        Call(List(i32), &l, PushBack, 1);
        Call(List(i32), &l, PushBack, 2);
        SORT_CMP(List(i32), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_i32(&l));
        TEST_ASSERT(r, *Call(List(i32), &l, Front) == 1);
        TEST_ASSERT(r, *Call(List(i32), &l, Back) == 3);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(i32) empty (no crash)");
    {
        List(i32) l;
        Create(List(i32), &l);
        SORT_CMP(List(i32), &l, NULL);
        TEST_ASSERT(r, Call(List(i32), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(i32) single element");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 99);
        SORT_CMP(List(i32), &l, NULL);
        TEST_ASSERT(r, *Call(List(i32), &l, Front) == 99);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(i32) already sorted");
    {
        List(i32) l;
        Create(List(i32), &l);
        for (i32 i = 0; i < 10; i++) Call(List(i32), &l, PushBack, i);
        SORT_CMP(List(i32), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_i32(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(i32) reverse sorted");
    {
        List(i32) l;
        Create(List(i32), &l);
        for (i32 i = 9; i >= 0; i--) Call(List(i32), &l, PushBack, i);
        SORT_CMP(List(i32), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_i32(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(i32) duplicates");
    {
        List(i32) l;
        Create(List(i32), &l);
        Call(List(i32), &l, PushBack, 5);
        Call(List(i32), &l, PushBack, 1);
        Call(List(i32), &l, PushBack, 3);
        Call(List(i32), &l, PushBack, 1);
        Call(List(i32), &l, PushBack, 5);
        SORT_CMP(List(i32), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_i32(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(i32) large scale (2000)");
    {
        List(i32) l;
        Create(List(i32), &l);
        for (i32 i = 1999; i >= 0; i--) Call(List(i32), &l, PushBack, i);
        SORT_CMP(List(i32), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_i32(&l));
        l.vptr->destroy(&l);
    }
}

// ============================================================
// List Sort Tests (String)
// ============================================================

static void _test_list_sort_string(TestRunner* r)
{
    TEST_GROUP(r, "List(String) ascending");
    {
        List(String) l;
        Create(List(String), &l);
        const char* words[] = { "banana", "apple", "cherry" };
        for (int i = 0; i < 3; i++)
        {
            String* s = format(words[i]);
            Call(List(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(List(String), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_String(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(String) empty (no crash)");
    {
        List(String) l;
        Create(List(String), &l);
        SORT_CMP(List(String), &l, NULL);
        TEST_ASSERT(r, Call(List(String), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(String) single element");
    {
        List(String) l;
        Create(List(String), &l);
        String* s = format("only");
        Call(List(String), &l, PushBack, *s);
        Call(String, s, Delete);
        SORT_CMP(List(String), &l, NULL);
        String* f = Call(List(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "only") == 0);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(String) already sorted");
    {
        List(String) l;
        Create(List(String), &l);
        const char* words[] = { "cat", "dog", "elephant" };
        for (int i = 0; i < 3; i++)
        {
            String* s = format(words[i]);
            Call(List(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(List(String), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_String(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(String) reverse sorted");
    {
        List(String) l;
        Create(List(String), &l);
        const char* words[] = { "zzz", "mmm", "aaa" };
        for (int i = 0; i < 3; i++)
        {
            String* s = format(words[i]);
            Call(List(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(List(String), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_String(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "List(String) duplicates");
    {
        List(String) l;
        Create(List(String), &l);
        const char* words[] = { "x", "a", "x", "b", "a" };
        for (int i = 0; i < 5; i++)
        {
            String* s = format(words[i]);
            Call(List(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(List(String), &l, NULL);
        TEST_ASSERT(r, _list_check_asc_String(&l));
        l.vptr->destroy(&l);
    }
}

// ============================================================
// DList Sort Tests (POD)
// ============================================================

static bool _dlist_check_asc_i32(const DList(i32)* l)
{
    if (l->size <= 1) return true;
    DListIter(i32) it = VCall(DList(i32), l, begin);
    DListIter(i32) end = VCall(DList(i32), l, end);
    i32 prev = VCall(DListIter(i32), &it, get);
    VCall(DListIter(i32), &it, next);
    while (!VCall(DListIter(i32), &it, equals, &end))
    {
        i32 cur = VCall(DListIter(i32), &it, get);
        if (prev > cur) return false;
        prev = cur;
        VCall(DListIter(i32), &it, next);
    }
    return true;
}

static bool _dlist_check_asc_String(const DList(String)* l)
{
    if (l->size <= 1) return true;
    DListIter(String) it = VCall(DList(String), l, begin);
    DListIter(String) end = VCall(DList(String), l, end);
    String prev = VCall(DListIter(String), &it, get);
    VCall(DListIter(String), &it, next);
    while (!VCall(DListIter(String), &it, equals, &end))
    {
        String cur = VCall(DListIter(String), &it, get);
        if (strcmp(prev.data, cur.data) > 0) return false;
        prev = cur;
        VCall(DListIter(String), &it, next);
    }
    return true;
}

static void _test_dlist_sort_pod(TestRunner* r)
{
    TEST_GROUP(r, "DList(i32) ascending");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 3);
        Call(DList(i32), &l, PushBack, 1);
        Call(DList(i32), &l, PushBack, 2);
        SORT_CMP(DList(i32), &l, NULL);
        TEST_ASSERT(r, _dlist_check_asc_i32(&l));
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 1);
        TEST_ASSERT(r, *Call(DList(i32), &l, Back) == 3);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(i32) empty (no crash)");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        SORT_CMP(DList(i32), &l, NULL);
        TEST_ASSERT(r, Call(DList(i32), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(i32) single element");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        Call(DList(i32), &l, PushBack, 99);
        SORT_CMP(DList(i32), &l, NULL);
        TEST_ASSERT(r, *Call(DList(i32), &l, Front) == 99);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(i32) already sorted");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        for (i32 i = 0; i < 10; i++) Call(DList(i32), &l, PushBack, i);
        SORT_CMP(DList(i32), &l, NULL);
        TEST_ASSERT(r, _dlist_check_asc_i32(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(i32) reverse sorted");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        for (i32 i = 9; i >= 0; i--) Call(DList(i32), &l, PushBack, i);
        SORT_CMP(DList(i32), &l, NULL);
        TEST_ASSERT(r, _dlist_check_asc_i32(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(i32) duplicates");
    {
        DList(i32) list;
        Create(DList(i32), &list);
        Call(DList(i32), &list, PushBack, 5);
        Call(DList(i32), &list, PushBack, 1);
        Call(DList(i32), &list, PushBack, 3);
        Call(DList(i32), &list, PushBack, 1);
        Call(DList(i32), &list, PushBack, 5);
        SORT(DList(i32), &list);
        TEST_ASSERT(r, _dlist_check_asc_i32(&list));

        foreach(DList(i32), i, list)
            print(i, " ");
        println();

        list.vptr->destroy(&list);
    }

    TEST_GROUP(r, "DList(i32) large scale (2000)");
    {
        DList(i32) l;
        Create(DList(i32), &l);
        for (i32 i = 1999; i >= 0; i--) Call(DList(i32), &l, PushBack, i);
        SORT_CMP(DList(i32), &l, NULL);
        TEST_ASSERT(r, _dlist_check_asc_i32(&l));
        l.vptr->destroy(&l);
    }
}

// ============================================================
// DList Sort Tests (String)
// ============================================================

static void _test_dlist_sort_string(TestRunner* r)
{
    TEST_GROUP(r, "DList(String) ascending");
    {
        DList(String) l;
        Create(DList(String), &l);
        const char* words[] = { "banana", "apple", "cherry" };
        for (int i = 0; i < 3; i++)
        {
            String* s = format(words[i]);
            Call(DList(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(DList(String), &l, NULL);
        TEST_ASSERT(r, _dlist_check_asc_String(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(String) empty (no crash)");
    {
        DList(String) l;
        Create(DList(String), &l);
        SORT_CMP(DList(String), &l, NULL);
        TEST_ASSERT(r, Call(DList(String), &l, IsEmpty));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(String) single element");
    {
        DList(String) l;
        Create(DList(String), &l);
        String* s = format("only");
        Call(DList(String), &l, PushBack, *s);
        Call(String, s, Delete);
        SORT_CMP(DList(String), &l, NULL);
        String* f = Call(DList(String), &l, Front);
        TEST_ASSERT(r, f && strcmp(f->data, "only") == 0);
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(String) already sorted");
    {
        DList(String) l;
        Create(DList(String), &l);
        const char* words[] = { "cat", "dog", "elephant" };
        for (int i = 0; i < 3; i++)
        {
            String* s = format(words[i]);
            Call(DList(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(DList(String), &l, NULL);
        TEST_ASSERT(r, _dlist_check_asc_String(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(String) reverse sorted");
    {
        DList(String) l;
        Create(DList(String), &l);
        const char* words[] = { "zzz", "mmm", "aaa" };
        for (int i = 0; i < 3; i++)
        {
            String* s = format(words[i]);
            Call(DList(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(DList(String), &l, NULL);
        TEST_ASSERT(r, _dlist_check_asc_String(&l));
        l.vptr->destroy(&l);
    }

    TEST_GROUP(r, "DList(String) duplicates");
    {
        DList(String) l;
        Create(DList(String), &l);
        const char* words[] = { "x", "a", "x", "b", "a" };
        for (int i = 0; i < 5; i++)
        {
            String* s = format(words[i]);
            Call(DList(String), &l, PushBack, *s);
            Call(String, s, Delete);
        }
        SORT_CMP(DList(String), &l, NULL);
        TEST_ASSERT(r, _dlist_check_asc_String(&l));
        l.vptr->destroy(&l);
    }
}

// ============================================================
// 主入口
// ============================================================

void algorithm_test(void)
{
    TEST_INIT(runner, "Algorithm Tests");
    TEST_BEGIN(&runner);

    #ifdef ALGORITHM_TEST_POD
    _test_vector_sort_pod(&runner);
    _test_list_sort_pod(&runner);
    _test_dlist_sort_pod(&runner);
    #endif
    #ifdef ALGORITHM_TEST_STRING
    _test_vector_sort_string(&runner);
    _test_list_sort_string(&runner);
    _test_dlist_sort_string(&runner);
    #endif

    TEST_END(&runner);
}

#endif