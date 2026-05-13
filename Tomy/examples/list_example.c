/**
 * list_example.c — 单向链表 List 使用示例
 *
 * 单链表，每个节点独立 malloc。支持 fromt/back 操作、
 * BeforeBegin 哨兵迭代器模式实现 InsertAfter/EraseAfter。
 *
 * 编译: cl list_example.c /I../Tomy/include /Fe:list_example.exe
 * 运行: ./list_example
 */

#include "tomy.h"

int main(void)
{
    /* ========== 1. POD 类型：List(i32) ========== */
    println("=== List(i32) 基础操作 ===");

    List(i32) l;
    Create(List(i32), &l);

    /* 尾部追加 */
    Call(List(i32), &l, PushBack, 10);
    Call(List(i32), &l, PushBack, 20);
    Call(List(i32), &l, PushBack, 30);
    println("size:", Call(List(i32), &l, Size));

    /* 首尾访问 */
    i32* f = Call(List(i32), &l, Front);
    i32* b = Call(List(i32), &l, Back);
    println("front:", *f, "back:", *b);

    /* 头部插入与移除 */
    Call(List(i32), &l, PushFront, 0);
    println("after push_front, front:", *Call(List(i32), &l, Front));

    Call(List(i32), &l, PopFront);
    println("after pop_front, front:", *Call(List(i32), &l, Front));

    /* foreach 迭代 */
    print("foreach:");
    foreach(List(i32), val, l)
    {
        print(" ", val);
    }
    println();

    /* InsertAfter / EraseAfter 配合 BeforeBegin */
    ListIter(i32) bb = Call(List(i32), &l, BeforeBegin);
    Call(List(i32), &l, InsertAfter, bb, 5);
    print("after BeforeBegin insert:");
    foreach(List(i32), val, l)
    {
        print(" ", val);
    }
    println();

    ListIter(i32) it = Call(List(i32), &l, Begin);
    Call(List(i32), &l, InsertAfter, it, 15);
    print("after insert after first:");
    foreach(List(i32), val, l)
    {
        print(" ", val);
    }
    println();

    /* 反转 */
    Call(List(i32), &l, Reverse);
    print("reversed:");
    foreach(List(i32), val, l)
    {
        print(" ", val);
    }
    println();

    /* 排序（归并排序） */
    SORT(List(i32), &l);
    print("sorted:");
    foreach(List(i32), val, l)
    {
        print(" ", val);
    }
    println();

    l.vptr->destroy(&l);

    /* ========== 2. String 类型 ========== */
    println("\n=== List(String) ===");

    List(String) sl;
    Create(List(String), &sl);

    String* ss[] = { format("banana"), format("apple"), format("cherry") };
    for (int i = 0; i < 3; i++)
    {
        Call(List(String), &sl, PushBack, *ss[i]);
        Call(String, ss[i], Delete);
    }

    print("String list:");
    foreach(List(String), str, sl)
    {
        print(" ", str);
    }
    println();

    SORT(List(String), &sl);
    print("sorted:");
    foreach(List(String), str, sl)
    {
        print(" ", str);
    }
    println();

    sl.vptr->destroy(&sl);

    return 0;
}