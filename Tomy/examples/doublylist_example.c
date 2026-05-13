/**
 * doublylist_example.c — 双向链表 DoublyList 使用示例
 *
 * 循环哨兵 sentinel 实现，O(1) 双向操作。
 * 支持 PushFront/PushBack/PopFront/PopBack，
 * Insert（在迭代器之前插入）、Erase。
 *
 * 编译: cl doublylist_example.c /I../Tomy/include /Fe:doublylist_example.exe
 * 运行: ./doublylist_example
 */

#include "tomy.h"

int main(void)
{
    /* ========== 1. POD 类型：DList(i32) ========== */
    println("=== DList(i32) 基础操作 ===");

    DList(i32) l;
    Create(DList(i32), &l);

    /* 双向操作 */
    Call(DList(i32), &l, PushBack, 10);
    Call(DList(i32), &l, PushBack, 30);
    Call(DList(i32), &l, PushFront, 5);
    println("size:", Call(DList(i32), &l, Size));

    i32* f = Call(DList(i32), &l, Front);
    i32* b = Call(DList(i32), &l, Back);
    println("front:", *f, "back:", *b);

    /* PopBack */
    Call(DList(i32), &l, PopBack);
    println("after pop_back, back:", *Call(DList(i32), &l, Back));

    /* foreach 迭代 */
    print("foreach:");
    foreach (DList(i32), val, l) { print(" ", val); }
    println();

    /* Insert: 在迭代器位置之前插入 */
    DListIter(i32) it = VCall(DList(i32), &l, begin);
    VCall(DListIter(i32), &it, next); /* 移动到第二个元素 */
    Call(DList(i32), &l, Insert, it, 20);
    print("after insert:");
    foreach (DList(i32), val, l) { print(" ", val); }
    println();

    /* 反转 */
    Call(DList(i32), &l, Reverse);
    print("reversed:");
    foreach (DList(i32), val, l) { print(" ", val); }
    println();

    /* 排序 */
    Call(DList(i32), &l, PushBack, 0);
    Call(DList(i32), &l, PushBack, 25);
    SORT(DList(i32), &l);
    print("sorted:");
    foreach (DList(i32), val, l) { print(" ", val); }
    println();

    /* 手动迭代器 */
    DListIter(i32) beg = VCall(DList(i32), &l, begin);
    DListIter(i32) end = VCall(DList(i32), &l, end);
    print("manual iteration:");
    while (!VCall(DListIter(i32), &beg, equals, &end))
    {
        print(" ", VCall(DListIter(i32), &beg, get));
        VCall(DListIter(i32), &beg, next);
    }
    println();

    l.vptr->destroy(&l);

    /* ========== 2. String 类型 ========== */
    println("\n=== DList(String) ===");

    DList(String) sl;
    Create(DList(String), &sl);

    String* words[] = { format("cat"), format("dog"), format("bird") };
    for (int i = 0; i < 3; i++)
    {
        Call(DList(String), &sl, PushBack, *words[i]);
        Call(String, words[i], Delete);
    }

    print("String dlist:");
    foreach (DList(String), str, sl) { print(" ", str.data); }
    println();

    Call(DList(String), &sl, Reverse);
    print("reversed:");
    foreach (DList(String), str, sl) { print(" ", str.data); }
    println();

    sl.vptr->destroy(&sl);

    return 0;
}
