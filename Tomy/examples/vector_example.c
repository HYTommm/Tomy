/**
 * vector_example.c — 泛型动态数组 Vector 使用示例
 *
 * 编译: cl vector_example.c /I../Tomy/include /Fe:vector_example.exe
 * 运行: ./vector_example
 */

#include "tomy.h"

int main(void)
{
    /* ========== 1. POD 类型：Vec(f32) ========== */
    println("=== Vec(f32) 基础操作 ===");

    Vec(f32) v;
    Create(Vec(f32), &v);

    /* 尾部追加 */
    FT(Vec(f32), &v)->push_back(&v, 1.0f);
    FT(Vec(f32), &v)->push_back(&v, 3.0f);
    FT(Vec(f32), &v)->push_back(&v, 2.0f);

    /* 索引访问 */
    f32* a0 = FT(Vec(f32), &v)->at(&v, 0);
    f32* a1 = FT(Vec(f32), &v)->at(&v, 1);
    f32* a2 = FT(Vec(f32), &v)->at(&v, 2);
    println("at(0):", *a0, "at(1):", *a1, "at(2):", *a2);

    /* 首尾元素 */
    f32* f = FT(Vec(f32), &v)->front(&v);
    f32* b = FT(Vec(f32), &v)->back(&v);
    println("front:", *f, "back:", *b);

    /* 尾部移除 */
    FT(Vec(f32), &v)->pop_back(&v);
    println("after pop_back, back:", *FT(Vec(f32), &v)->back(&v));

    /* foreach 迭代 */
    print("foreach:");
    foreach(Vec(f32), val, v)
    {
        print(" ", val);
    }
    println();

    /* 排序 */
    FT(Vec(f32), &v)->push_back(&v, 5.0f);
    FT(Vec(f32), &v)->push_back(&v, 0.0f);
    SORT_CMP(Vec(f32), &v, NULL);
    print("sorted:");
    foreach(Vec(f32), val, v)
    {
        print(" ", val);
    }
    println();

    /* 插入和删除 */
    FT(Vec(f32), &v)->insert(&v, 2, 99.0f);
    println("after insert at 2:", *FT(Vec(f32), &v)->at(&v, 2));

    FT(Vec(f32), &v)->erase(&v, 2);
    println("size after erase:", VCall(Vec(f32), &v, is_empty));

    /* emplace_back: 原地构造 */
    f32* emp = (f32*)FT(Vec(f32), &v)->emplace_back(&v);
    *emp = 42.0f;
    println("emplace_back value:", *FT(Vec(f32), &v)->back(&v));

    /* 手动迭代器 */
    VecIter(f32) it = VCall(Vec(f32), &v, begin);
    VecIter(f32) end_it = VCall(Vec(f32), &v, end);
    print("manual iterator:");
    while (!VCall(VecIter(f32), &it, equals, &end_it))
    {
        print(" ", VCall(VecIter(f32), &it, get));
        VCall(VecIter(f32), &it, next);
    }
    println();

    v.vptr->destroy(&v);

    /* ========== 2. String 类型 ========== */
    println("\n=== Vec(String) ===");

    Vec(String) sv;
    Create(Vec(String), &sv);

    String* s1 = format("hello");
    String* s2 = format("world");
    String* s3 = format("tomy");
    FT(Vec(String), &sv)->push_back(&sv, *s1);
    FT(Vec(String), &sv)->push_back(&sv, *s2);
    FT(Vec(String), &sv)->push_back(&sv, *s3);
    Call(String, s1, Delete);
    Call(String, s2, Delete);
    Call(String, s3, Delete);

    print("String vector:");
    foreach(Vec(String), str, sv)
    {
        print(" ", &str);
    }
    println();

    sv.vptr->destroy(&sv);

    /* ========== 3. reserve / resize ========== */
    println("\n=== reserve / resize ===");

    Vec(i32) iv;
    Create(Vec(i32), &iv);
    FT(Vec(i32), &iv)->reserve(&iv, 100);
    println("reserved capacity >= 100");

    FT(Vec(i32), &iv)->resize(&iv, 5);
    for (i32 i = 0; i < 5; i++)
        *FT(Vec(i32), &iv)->at(&iv, i) = i * 10;

    foreach(Vec(i32), val, iv)
    {
        print(" ", val);
    }
    println();

    iv.vptr->destroy(&iv);

    return 0;
}