#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

#ifdef RESULT_TEST

RESULT_IMPL(i32, i32)

/* 非 POD 类型：Result(i32, String) — String 作为错误类型 */
RESULT_IMPL_EX(i32, String, NULL, NULL, _String_Destroy, _String_Copy);

void result_test_pod(TestRunner* r)
{
    TEST_GROUP(r, "Ok / IsOk / IsErr");
    {
        Result(i32, i32) res = Result_Ok(i32, i32, 42);
        TEST_ASSERT(r, Result_IsOk(i32, i32, res));
        TEST_ASSERT(r, !Result_IsErr(i32, i32, res));
    }

    TEST_GROUP(r, "Err / IsOk / IsErr");
    {
        Result(i32, i32) res = Result_Err(i32, i32, -1);
        TEST_ASSERT(r, !Result_IsOk(i32, i32, res));
        TEST_ASSERT(r, Result_IsErr(i32, i32, res));
    }

    TEST_GROUP(r, "AsPtr ok");
    {
        Result(i32, i32) res = Result_Ok(i32, i32, 99);
        TEST_ASSERT_EQ(r, *Result_AsPtr(i32, i32, res), 99);
    }

    TEST_GROUP(r, "Destroy ok");
    {
        Result(i32, i32) res = Result_Ok(i32, i32, 42);
        Result_Destroy(i32, i32, &res);
        TEST_ASSERT(r, !res.ok);  /* Destroy sets ok = false */
    }

    TEST_GROUP(r, "Destroy err");
    {
        Result(i32, i32) res = Result_Err(i32, i32, -1);
        Result_Destroy(i32, i32, &res);
        TEST_ASSERT(r, !res.ok);
    }

    TEST_GROUP(r, "Multiple results independent");
    {
        Result(i32, i32) a = Result_Ok(i32, i32, 10);
        Result(i32, i32) b = Result_Err(i32, i32, 1);
        Result(i32, i32) c = Result_Ok(i32, i32, 20);
        TEST_ASSERT(r, Result_IsOk(i32, i32, a) && Result_IsErr(i32, i32, b) && Result_IsOk(i32, i32, c));
        TEST_ASSERT_EQ(r, *Result_AsPtr(i32, i32, a), 10);
        TEST_ASSERT_EQ(r, *Result_AsPtr(i32, i32, c), 20);
    }
}

void result_test_string(TestRunner* r)
{
    TEST_GROUP(r, "Err with String error");
    {
        String s;
        Create(String, &s);
        Call(String, &s, Append, "not found");

        Result(i32, String) res = Result_Err(i32, String, s);

        TEST_ASSERT(r, Result_IsErr(i32, String, res));
        TEST_ASSERT(r, !Result_IsOk(i32, String, res));
        TEST_ASSERT(r, strcmp(res.error.data, "not found") == 0);

        Result_Destroy(i32, String, &res);
        Call(String, &s, Destroy);
    }

    TEST_GROUP(r, "Ok with String error type (no error set)");
    {
        Result(i32, String) res = Result_Ok(i32, String, 99);
        TEST_ASSERT(r, Result_IsOk(i32, String, res));
        TEST_ASSERT_EQ(r, *Result_AsPtr(i32, String, res), 99);
        Result_Destroy(i32, String, &res);
    }

    TEST_GROUP(r, "Destroy idempotent");
    {
        String s;
        Create(String, &s);
        Result(i32, String) res = Result_Err(i32, String, s);
        Result_Destroy(i32, String, &res);
        Result_Destroy(i32, String, &res);  /* second call: no-op */
        TEST_ASSERT(r, Result_IsErr(i32, String, res));
        Call(String, &s, Destroy);
    }
}

void result_test(void)
{
    TEST_INIT(runner, "Result Tests");
    TEST_BEGIN(&runner);

#ifdef RESULT_TEST_POD
    result_test_pod(&runner);
#endif
#ifdef RESULT_TEST_STRING
    result_test_string(&runner);
#endif

    TEST_END(&runner);
}

#endif
