#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

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
