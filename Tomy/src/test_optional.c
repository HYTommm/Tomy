#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef OPTIONAL_TEST

/* 非 POD 类型：String */
OPTIONAL_IMPL_EX(String, _String_Destroy, _String_Copy);

void optional_test_pod(TestRunner* r)
{
    TEST_GROUP(r, "Some / IsSome / IsNone");
    {
        Optional(i32) opt = Optional_Some(i32, 42);
        TEST_ASSERT(r, Optional_IsSome(i32, opt));
        TEST_ASSERT(r, !Optional_IsNone(i32, opt));
    }

    TEST_GROUP(r, "None / IsSome / IsNone");
    {
        Optional(i32) opt = Optional_None(i32);
        TEST_ASSERT(r, !Optional_IsSome(i32, opt));
        TEST_ASSERT(r, Optional_IsNone(i32, opt));
    }

    TEST_GROUP(r, "AsPtr deref");
    {
        Optional(i32) opt = Optional_Some(i32, 99);
        TEST_ASSERT_EQ(r, *Optional_AsPtr(i32, opt), 99);
    }

    TEST_GROUP(r, "AsPtr some");
    {
        Optional(i32) opt = Optional_Some(i32, 77);
        i32* p = Optional_AsPtr(i32, opt);
        TEST_ASSERT_NOT_NULL(r, p);
        TEST_ASSERT_EQ(r, *p, 77);
    }

    TEST_GROUP(r, "AsPtr none");
    {
        Optional(i32) opt = Optional_None(i32);
        TEST_ASSERT_NULL(r, Optional_AsPtr(i32, opt));
    }

    TEST_GROUP(r, "Destroy after Some");
    {
        Optional(i32) opt = Optional_Some(i32, 42);
        Optional_Destroy(i32, &opt);
        TEST_ASSERT(r, Optional_IsNone(i32, opt));
    }

    TEST_GROUP(r, "Destroy after None");
    {
        Optional(i32) opt = Optional_None(i32);
        Optional_Destroy(i32, &opt);   /* no-op, shouldn't crash */
        TEST_ASSERT(r, Optional_IsNone(i32, opt));
    }

    TEST_GROUP(r, "Reset");
    {
        Optional(i32) opt = Optional_Some(i32, 10);
        Optional_Reset(i32, &opt, 20);
        TEST_ASSERT(r, Optional_IsSome(i32, opt));
        TEST_ASSERT_EQ(r, *Optional_AsPtr(i32, opt), 20);
    }

    TEST_GROUP(r, "Copy POD");
    {
        Optional(i32) a = Optional_Some(i32, 42);
        Optional(i32) b = Optional_None(i32);
        Optional_Copy(i32, &b, &a);
        TEST_ASSERT(r, Optional_IsSome(i32, b));
        TEST_ASSERT_EQ(r, *Optional_AsPtr(i32, b), 42);
    }

    TEST_GROUP(r, "Copy self");
    {
        Optional(i32) a = Optional_Some(i32, 7);
        Optional_Copy(i32, &a, &a);  /* no-op, shouldn't crash */
        TEST_ASSERT(r, Optional_IsSome(i32, a));
        TEST_ASSERT_EQ(r, *Optional_AsPtr(i32, a), 7);
    }

    TEST_GROUP(r, "f64 type");
    {
        Optional(f64) opt = Optional_Some(f64, 3.14);
        TEST_ASSERT(r, Optional_IsSome(f64, opt));
        TEST_ASSERT(r, *Optional_AsPtr(f64, opt) > 3.13 && *Optional_AsPtr(f64, opt) < 3.15);
    }

    TEST_GROUP(r, "Multiple optionals independent");
    {
        Optional(i32) a = Optional_Some(i32, 10);
        Optional(i32) b = Optional_None(i32);
        Optional(i32) c = Optional_Some(i32, 20);
        TEST_ASSERT(r, Optional_IsSome(i32, a) && Optional_IsNone(i32, b) && Optional_IsSome(i32, c));
        TEST_ASSERT_EQ(r, *Optional_AsPtr(i32, a), 10);
        TEST_ASSERT_EQ(r, *Optional_AsPtr(i32, c), 20);
    }
}

void optional_test_string(TestRunner* r)
{
    TEST_GROUP(r, "Some String");
    {
        String s;
        Create(String, &s);
        Call(String, &s, Append, "hello");

        Optional(String) opt = Optional_Some(String, s);

        TEST_ASSERT(r, Optional_IsSome(String, opt));
        TEST_ASSERT_STR_EQ(r, Optional_AsPtr(String, &opt)->data, "hello");

        Optional_Destroy(String, &opt);
        TEST_ASSERT(r, Optional_IsNone(String, opt));

        Call(String, &s, Destroy);
    }

    TEST_GROUP(r, "None String (no crash)");
    {
        Optional(String) opt = Optional_None(String);
        TEST_ASSERT(r, Optional_IsNone(String, opt));
        Optional_Destroy(String, &opt);
    }

    TEST_GROUP(r, "Reset String");
    {
        String a, b;
        Create(String, &a);
        Call(String, &a, Append, "first");
        Create(String, &b);
        Call(String, &b, Append, "second");

        Optional(String) opt = Optional_Some(String, a);
        Optional_Reset(String, &opt, b);

        TEST_ASSERT_STR_EQ(r, Optional_AsPtr(String, &opt)->data, "second");

        Optional_Destroy(String, &opt);
        Call(String, &a, Destroy);
        Call(String, &b, Destroy);
    }

    TEST_GROUP(r, "Copy String");
    {
        String s;
        Create(String, &s);
        Call(String, &s, Append, "hello");

        Optional(String) a = Optional_Some(String, s);
        Optional(String) b = Optional_None(String);

        Optional_Copy(String, &b, &a);
        TEST_ASSERT(r, Optional_IsSome(String, b));
        TEST_ASSERT_STR_EQ(r, Optional_AsPtr(String, &b)->data, "hello");
        /* b 是独立深拷贝，Destroy a 不影响 b */
        Optional_Destroy(String, &a);
        TEST_ASSERT(r, Optional_IsNone(String, a));
        TEST_ASSERT_STR_EQ(r, Optional_AsPtr(String, &b)->data, "hello");
        Optional_Destroy(String, &b);

        Call(String, &s, Destroy);
    }

    TEST_GROUP(r, "Destroy idempotent");
    {
        Optional(String) opt = Optional_None(String);
        Optional_Destroy(String, &opt);
        Optional_Destroy(String, &opt);
        TEST_ASSERT(r, Optional_IsNone(String, opt));
    }
}

void optional_test(void)
{
    TEST_INIT(runner, "Optional Tests");
    TEST_BEGIN(&runner);

#ifdef OPTIONAL_TEST_POD
    optional_test_pod(&runner);
#endif
#ifdef OPTIONAL_TEST_STRING
    optional_test_string(&runner);
#endif

    TEST_END(&runner);
}

#endif
