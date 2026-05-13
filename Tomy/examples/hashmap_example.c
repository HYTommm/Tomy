/**
 * hashmap_example.c — 哈希表 HashMap 使用示例
 *
 * 开放寻址 + 线性探测，负载因子 0.75。
 * 支持 Insert/At/Erase/Contains/Find/迭代器/foreach。
 *
 * 编译: cl hashmap_example.c /I../Tomy/include /Fe:hashmap_example.exe
 * 运行: ./hashmap_example
 */

#include "tomy.h"

int main(void)
{
    /* ========== 1. POD Key-Value：HashMap(i32, f64) ========== */
    println("=== HashMap(i32, f64) 基础操作 ===");

    HashMap(i32, f64) m;
    Create(HashMap(i32, f64), &m);

    /* 插入 */
    Call(HashMap(i32, f64), &m, Insert, 1, 10.5);
    Call(HashMap(i32, f64), &m, Insert, 2, 20.5);
    Call(HashMap(i32, f64), &m, Insert, 3, 30.5);
    println("size:", Call(HashMap(i32, f64), &m, Size));

    /* 查找 */
    f64* v1 = Call(HashMap(i32, f64), &m, At, 1);
    f64* v2 = Call(HashMap(i32, f64), &m, At, 2);
    println("at(1):", *v1, "at(2):", *v2);

    /* 不存在的键返回 NULL */
    f64* vn = Call(HashMap(i32, f64), &m, At, 99);
    println("at(99) is NULL:", vn == NULL);

    /* Contains */
    println("contains(1):", Call(HashMap(i32, f64), &m, Contains, 1));
    println("contains(99):", Call(HashMap(i32, f64), &m, Contains, 99));

    /* Insert overwrite */
    Call(HashMap(i32, f64), &m, Insert, 1, 100.0);
    println("overwrite key=1:", *Call(HashMap(i32, f64), &m, At, 1));

    /* Erase */
    Call(HashMap(i32, f64), &m, Erase, 2);
    println("after erase(2), size:", Call(HashMap(i32, f64), &m, Size));
    println("contains(2):", Call(HashMap(i32, f64), &m, Contains, 2));

    /* foreach 迭代（Pair 结构体：key/value 指针） */
    print("foreach:");
    foreach(HashMap(i32, f64), pair, m)
    {
        print(" {", *pair.key, ":", *pair.value, "}");
    }
    println();

    /* 手动迭代器 */
    HashMapIter(i32, f64) it = VCall(HashMap(i32, f64), &m, begin);
    HashMapIter(i32, f64) end = VCall(HashMap(i32, f64), &m, end);
    print("manual iterator:");
    while (!VCall(HashMapIter(i32, f64), &it, equals, &end))
    {
        i32* k = VCall(HashMapIter(i32, f64), &it, key);
        f64* v = VCall(HashMapIter(i32, f64), &it, value);
        print(" {", *k, ":", *v, "}");
        VCall(HashMapIter(i32, f64), &it, next);
    }
    println();

    /* Find 返回迭代器 */
    HashMapIter(i32, f64) found = Call(HashMap(i32, f64), &m, Find, 3);
    println("find(3): key=", *VCall(HashMapIter(i32, f64), &found, key),
        "value=", *VCall(HashMapIter(i32, f64), &found, value));

/* LoadFactor */
    println("load_factor:", Call(HashMap(i32, f64), &m, LoadFactor));

    /* TryEmplace — 仅键不存在时插入 */
    i32 new_key = 4;
    f64 new_val = 40.0;
    bool inserted = Call(HashMap(i32, f64), &m, TryEmplace, &new_key, &new_val);
    println("try_emplace(4):", inserted, "value:", *Call(HashMap(i32, f64), &m, At, 4));

    i32 exist_key = 1;
    f64 overwrite = 999.0;
    inserted = Call(HashMap(i32, f64), &m, TryEmplace, &exist_key, &overwrite);
    println("try_emplace(1) again (should fail):", inserted);

    /* Clear */
    Call(HashMap(i32, f64), &m, Clear);
    println("after clear, size:", Call(HashMap(i32, f64), &m, Size));

    m.vptr->destroy(&m);

    /* ========== 2. String 键 ========== */
    println("\n=== HashMap(String, i32) ===");

    HashMap(String, i32) sm;
    Create(HashMap(String, i32), &sm);

    String* sk1 = New(String, 16);
    Call(String, sk1, Append, "apple");
    String* sk2 = New(String, 16);
    Call(String, sk2, Append, "banana");

    Call(HashMap(String, i32), &sm, Insert, *sk1, 100);
    Call(HashMap(String, i32), &sm, Insert, *sk2, 200);

    i32* sv = Call(HashMap(String, i32), &sm, At, *sk1);
    println("at(apple):", *sv);

    Call(String, sk1, Delete);
    Call(String, sk2, Delete);

    foreach(HashMap(String, i32), p, sm)
    {
        println(" ", p.key, "->", *p.value);
    }

    sm.vptr->destroy(&sm);

    return 0;
}