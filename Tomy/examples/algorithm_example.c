/**
 * algorithm_example.c — 算法（排序/查找/统计/反转）使用示例
 *
 * 适用容器：Vec, List, DList
 * 使用 SORT/SORT_CMP 宏进行排序。
 *
 * 编译: cl algorithm_example.c /I../Tomy/include /Fe:algorithm_example.exe
 * 运行: ./algorithm_example
 */

#include "tomy.h"

/* 降序比较器 */
static int cmp_desc(const i32* a, const i32* b)
{
    return (*a < *b) - (*a > *b);
}

/* 谓词：偶数 */
static bool is_even(const i32* x) { return *x % 2 == 0; }

int main(void)
{
    /* ========== 1. Vector 排序 ========== */
    println("=== Vec(i32) 排序 ===");

    Vec(i32) v;
    Create(Vec(i32), &v);

    FT(Vec(i32), &v)->push_back(&v, 5);
    FT(Vec(i32), &v)->push_back(&v, 2);
    FT(Vec(i32), &v)->push_back(&v, 8);
    FT(Vec(i32), &v)->push_back(&v, 1);
    FT(Vec(i32), &v)->push_back(&v, 9);

    /* 默认升序 */
    SORT(Vec(i32), &v);
    print("Vec ascending:");
    foreach (Vec(i32), val, v) { print(" ", val); }
    println();

    /* 自定义降序 */
    FT(Vec(i32), &v)->push_back(&v, 3);
    FT(Vec(i32), &v)->push_back(&v, 7);
    SORT_CMP(Vec(i32), &v, cmp_desc);
    print("Vec descending:");
    foreach (Vec(i32), val, v) { print(" ", val); }
    println();

    v.vptr->destroy(&v);

    /* ========== 2. List 排序 ========== */
    println("\n=== List(i32) 排序 ===");

    List(i32) l;
    Create(List(i32), &l);

    Call(List(i32), &l, PushBack, 9);
    Call(List(i32), &l, PushBack, 1);
    Call(List(i32), &l, PushBack, 7);
    Call(List(i32), &l, PushBack, 3);

    SORT(List(i32), &l);
    print("List ascending:");
    foreach (List(i32), val, l) { print(" ", val); }
    println();

    /* 降序 */
    SORT_CMP(List(i32), &l, cmp_desc);
    print("List descending:");
    foreach (List(i32), val, l) { print(" ", val); }
    println();

    l.vptr->destroy(&l);

    /* ========== 3. DList 排序 ========== */
    println("\n=== DList(i32) 排序 ===");

    DList(i32) dl;
    Create(DList(i32), &dl);

    Call(DList(i32), &dl, PushBack, 4);
    Call(DList(i32), &dl, PushBack, 6);
    Call(DList(i32), &dl, PushBack, 2);
    Call(DList(i32), &dl, PushBack, 8);

    SORT(DList(i32), &dl);
    print("DList ascending:");
    foreach (DList(i32), val, dl) { print(" ", val); }
    println();

    dl.vptr->destroy(&dl);

    /* ========== 4. String 排序 ========== */
    println("\n=== Vec(String) 排序 ===");

    Vec(String) sv;
    Create(Vec(String), &sv);

    const char* fruits[] = { "banana", "apple", "cherry", "date" };
    for (int i = 0; i < 4; i++)
    {
        String* s = format(fruits[i]);
        FT(Vec(String), &sv)->push_back(&sv, *s);
        Call(String, s, Delete);
    }

    SORT(Vec(String), &sv);
    print("sorted strings:");
    foreach (Vec(String), str, sv) { print(" ", str.data); }
    println();

    sv.vptr->destroy(&sv);

    /* ========== 5. 反转 ========== */
    println("\n=== List(i32) 反转 ===");

    List(i32) rl;
    Create(List(i32), &rl);
    for (i32 i = 1; i <= 5; i++)
        Call(List(i32), &rl, PushBack, i);

    print("original:");
    foreach (List(i32), val, rl) { print(" ", val); }
    println();

    Call(List(i32), &rl, Reverse);
    print("reversed:");
    foreach (List(i32), val, rl) { print(" ", val); }
    println();

    rl.vptr->destroy(&rl);

    /* ========== 6. 迭代器 ========== */
    println("\n=== Vec(f32) 迭代器遍历 ===");

    Vec(f32) fv;
    Create(Vec(f32), &fv);
    for (f32 i = 0; i < 5; i++)
        FT(Vec(f32), &fv)->push_back(&fv, i * 1.5f);

    f32 sum = 0;
    foreach (Vec(f32), val, fv) { sum += val; }
    println("foreach sum:", sum);

    /* 手动迭代器 */
    VecIter(f32) vit = VCall(Vec(f32), &fv, begin);
    VecIter(f32) vend = VCall(Vec(f32), &fv, end);
    f32 prod = 1;
    while (!VCall(VecIter(f32), &vit, equals, &vend))
    {
        prod *= VCall(VecIter(f32), &vit, get);
        VCall(VecIter(f32), &vit, next);
    }
    println("iterator product:", prod);

    fv.vptr->destroy(&fv);

    /* ========== 7. 查找 ========== */
    println("\n=== Vec(i32) 查找 ===");

    Vec(i32) sv;
    Create(Vec(i32), &sv);
    for (i32 i = 10; i <= 50; i += 10)
        FT(Vec(i32), &sv)->push_back(&sv, i);

    i32* found = FIND(Vec(i32), &sv, (i32)30);
    if (found) println("Find 30:", *found);

    found = FIND(Vec(i32), &sv, (i32)99);
    if (!found) println("Find 99: not found");

    /* FindIf */
    found = FIND_IF(Vec(i32), &sv, is_even);
    if (found) println("First even:", *found);

    sv.vptr->destroy(&sv);

    /* ========== 8. 二分查找（需要排序）========== */
    println("\n=== Vec(i32) 二分查找 ===");

    Vec(i32) bv;
    Create(Vec(i32), &bv);
    for (i32 i = 1; i <= 10; i++)
        FT(Vec(i32), &bv)->push_back(&bv, i);

    i32* bs = BINARY_SEARCH(Vec(i32), &bv, (i32)7);
    println("BinarySearch 7:", bs ? *bs : -1);

    bs = BINARY_SEARCH(Vec(i32), &bv, (i32)99);
    println("BinarySearch 99:", bs ? "found" : "not found");

    /* LowerBound / UpperBound */
    FT(Vec(i32), &bv)->push_back(&bv, 7);
    FT(Vec(i32), &bv)->push_back(&bv, 7);
    SORT(Vec(i32), &bv);

    i32* lb = LOWER_BOUND(Vec(i32), &bv, (i32)7);
    i32* ub = UPPER_BOUND(Vec(i32), &bv, (i32)7);
    println("7 appears", (umax)(ub - lb), "times");

    bv.vptr->destroy(&bv);

    /* ========== 9. 计数 ========== */
    println("\n=== Vec(i32) 计数 ===");

    Vec(i32) cv;
    Create(Vec(i32), &cv);
    int cv_data[] = { 1, 2, 2, 3, 2, 4, 2 };
    for (int i = 0; i < 7; i++)
        FT(Vec(i32), &cv)->push_back(&cv, cv_data[i]);

    umax cnt = COUNT(Vec(i32), &cv, (i32)2);
    println("Count of 2:", cnt);
    println("Count of 5:", COUNT(Vec(i32), &cv, (i32)5));

    cv.vptr->destroy(&cv);

    /* ========== 10. 反转 ========== */
    println("\n=== Vec(i32) 反转 ===");

    Vec(i32) rv;
    Create(Vec(i32), &rv);
    for (i32 i = 1; i <= 6; i++)
        FT(Vec(i32), &rv)->push_back(&rv, i);

    print("before:");
    foreach (Vec(i32), val, rv) { print(" ", val); }
    println();

    REVERSE(Vec(i32), &rv);
    print("after:");
    foreach (Vec(i32), val, rv) { print(" ", val); }
    println();

    rv.vptr->destroy(&rv);

    return 0;
}
