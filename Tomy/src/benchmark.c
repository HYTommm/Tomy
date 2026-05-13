#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#include "tomy.h"
#include "test_config.h"

#ifdef BENCHMARK

/* ============ High-precision timer ============ */

typedef struct
{
    LARGE_INTEGER start;
    LARGE_INTEGER freq;
} Timer;

static inline void Timer_Start(Timer* t)
{
    QueryPerformanceFrequency(&t->freq);
    QueryPerformanceCounter(&t->start);
}
static inline double Timer_ElapsedMs(Timer* t)
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - t->start.QuadPart) * 1000.0 / (double)t->freq.QuadPart;
}

/* ============ Benchmark label width helper ============ */
/* prints "  LABEL............ " padded to 34 chars */
#define BENCH_LABEL(align_label) \
    do { \
        int _len = (int)strlen(align_label); \
        printf("  %s", align_label); \
        for (int _i = _len; _i < 34; ++_i) putchar('.'); \
        printf(" "); \
    } while(0)

/* Simple timing - no per-op calculation */
#define BENCH(label, code_block) \
    do { \
        Timer _t; \
        Timer_Start(&_t); \
        code_block; \
        double _ms = Timer_ElapsedMs(&_t); \
        BENCH_LABEL(label); \
        printf("%8.2f ms\n", _ms); \
    } while(0)

/* Timing with per-op: n = total number of operations in code_block */
#define BENCH_N(label, n, code_block) \
    do { \
        Timer _t; \
        Timer_Start(&_t); \
        code_block; \
        double _ms = Timer_ElapsedMs(&_t); \
        double _per = _ms / (double)(n); \
        BENCH_LABEL(label); \
        printf("%8.2f ms  (%7.2f ns/op)\n", _ms, _per * 1000000.0); \
    } while(0)

/* ============================================================
 * 大元素类型（64 字节）
 * ============================================================ */

typedef struct
{
    i64 data[8];
} BigElem;

static int _BigElem_cmp_default(const BigElem* a, const BigElem* b)
{
    return (a->data[0] > b->data[0]) - (a->data[0] < b->data[0]);
}

LIST_IMPL_EX(BigElem, NULL, NULL, NULL, _BigElem_cmp_default);
POOLLIST_IMPL_EX(BigElem, NULL, NULL, NULL, _BigElem_cmp_default);

/* ============================================================
 * PoolList vs List（单链表）
 * ============================================================ */

static void bench_singly(void)
{
    printf("\n========== Singly-Linked: List  vs  PoolList ==========\n");

    const int N = 200000;   /* 批量构造/遍历 */
    const int C = 50000;    /* cycle 反复次数 */

    /* ---- PushBack ---- */
    printf("\n--- PushBack %d ---\n", N);
    BENCH_N("List PushBack", N, {
        List(i32) l;
        Create(List(i32), &l);
        for (int i = 0; i < N; i++) Call(List(i32), &l, PushBack, i);
        Call(List(i32), &l, Destroy);
        });
    BENCH_N("PoolList PushBack", N, {
        PList(i32) l;
        Create(PList(i32), &l);
        for (int i = 0; i < N; i++) Call(PList(i32), &l, PushBack, i);
        Call(PList(i32), &l, Destroy);
        });

        /* ---- PushFront ---- */
    printf("\n--- PushFront %d ---\n", N);
    BENCH_N("List PushFront", N, {
        List(i32) l;
        Create(List(i32), &l);
        for (int i = 0; i < N; i++) Call(List(i32), &l, PushFront, i);
        Call(List(i32), &l, Destroy);
        });
    BENCH_N("PoolList PushFront", N, {
        PList(i32) l;
        Create(PList(i32), &l);
        for (int i = 0; i < N; i++) Call(PList(i32), &l, PushFront, i);
        Call(PList(i32), &l, Destroy);
        });

        /* ---- foreach 遍历 ---- */
    printf("\n--- Foreach %d (x5 rounds) ---\n", N);
    {
        List(i32) l1;
        Create(List(i32), &l1);
        for (int i = 0; i < N; i++) Call(List(i32), &l1, PushBack, i);
        volatile i32 s1 = 0;
        BENCH_N("List Foreach x5", 5, {
            for (int r = 0; r < 5; r++)
                foreach(List(i32), v, l1) s1 += v;
            });
        Call(List(i32), &l1, Destroy);

        PList(i32) l2;
        Create(PList(i32), &l2);
        for (int i = 0; i < N; i++) Call(PList(i32), &l2, PushBack, i);
        volatile i32 s2 = 0;
        BENCH_N("PoolList Foreach x5", 5, {
            for (int r = 0; r < 5; r++)
                foreach(PList(i32), v, l2) s2 += v;
            });
        Call(PList(i32), &l2, Destroy);
    }

    /* ---- 槽位复用：同一 list 上反复 push/pop ---- */
    {
        int total_ops = C * 100; /* push 50 + pop 50 per iter */
        printf("\n--- Slot reuse: push50 + pop50 x %d (same list) ---\n", C);
        BENCH_N("List slot reuse", total_ops, {
            List(i32) l;
            Create(List(i32), &l);
            for (int iter = 0; iter < C; iter++)
 {
for (int j = 0; j < 50; j++) Call(List(i32), &l, PushBack, j);
for (int j = 0; j < 50; j++) Call(List(i32), &l, PopFront);
}
Call(List(i32), &l, Destroy);
            });
        BENCH_N("PoolList slot reuse", total_ops, {
            PList(i32) l;
            Create(PList(i32), &l);
            for (int iter = 0; iter < C; iter++)
 {
for (int j = 0; j < 50; j++) Call(PList(i32), &l, PushBack, j);
for (int j = 0; j < 50; j++) Call(PList(i32), &l, PopFront);
}
Call(PList(i32), &l, Destroy);
            });
    }

    /* ---- Create/Destroy 反复 ---- */
    {
        int iter = 5000;
        printf("\n--- Create/Destroy x %d (push 100 then destroy) ---\n", iter);
        BENCH_N("List C/D", iter * 100, {
            for (int k = 0; k < iter; k++)
 {
List(i32) l;
Create(List(i32), &l);
for (int i = 0; i < 100; i++) Call(List(i32), &l, PushBack, i);
Call(List(i32), &l, Destroy);
}
            });
        BENCH_N("PoolList C/D", iter * 100, {
            for (int k = 0; k < iter; k++)
 {
PList(i32) l;
Create(PList(i32), &l);
for (int i = 0; i < 100; i++) Call(PList(i32), &l, PushBack, i);
Call(PList(i32), &l, Destroy);
}
            });
    }

    /* ---- FIFO: push all then pop all ---- */
    printf("\n--- FIFO: push %d then pop all ---\n", N);
    BENCH_N("List FIFO", N, {
        List(i32) l;
        Create(List(i32), &l);
        for (int i = 0; i < N; i++) Call(List(i32), &l, PushBack, i);
        while (!Call(List(i32), &l, IsEmpty))
            Call(List(i32), &l, PopFront);
        Call(List(i32), &l, Destroy);
        });
    BENCH_N("PoolList FIFO", N, {
        PList(i32) l;
        Create(PList(i32), &l);
        for (int i = 0; i < N; i++) Call(PList(i32), &l, PushBack, i);
        while (!Call(PList(i32), &l, IsEmpty))
            Call(PList(i32), &l, PopFront);
        Call(PList(i32), &l, Destroy);
        });

        /* ---- Reverse ---- */
    printf("\n--- Reverse %d ---\n", N);
    {
        List(i32) l1;
        Create(List(i32), &l1);
        for (int i = 0; i < N; i++) Call(List(i32), &l1, PushBack, i);
        BENCH("List Reverse", { Call(List(i32), &l1, Reverse); });
        Call(List(i32), &l1, Destroy);

        PList(i32) l2;
        Create(PList(i32), &l2);
        for (int i = 0; i < N; i++) Call(PList(i32), &l2, PushBack, i);
        BENCH("PoolList Reverse", { Call(PList(i32), &l2, Reverse); });
        Call(PList(i32), &l2, Destroy);
    }

    /* ---- 大元素 ---- */
    printf("\n--- BigElem(64B) PushBack %d ---\n", N);
    BENCH_N("List BigElem", N, {
        List(BigElem) l;
        Create(List(BigElem), &l);
        BigElem e = {{0}};
        for (int i = 0; i < N; i++)
        {
            e.data[0] = i; Call(List(BigElem), &l, PushBack, e);
        }
        Call(List(BigElem), &l, Destroy);
        });
    BENCH_N("PoolList BigElem", N, {
        PList(BigElem) l;
        Create(PList(BigElem), &l);
        BigElem e = {{0}};
        for (int i = 0; i < N; i++)
        {
            e.data[0] = i; Call(PList(BigElem), &l, PushBack, e);
        }
        Call(PList(BigElem), &l, Destroy);
        });
    /* ---- 顺序推进随机插入（基于上一次迭代器，只前进）---- */
    {
        int EXTRA = 2000;
        printf("\n--- Sequential random insert: build %d, insert %d, move forward random steps ---\n", N, EXTRA);

        // 预生成随机步数序列（值范围 1~50）
        int* steps = malloc(EXTRA * sizeof(int));
        srand(12345);
        for (int i = 0; i < EXTRA; i++)
        {
            steps[i] = (rand() % 50) + 1; // 步数 1~50
        }

        // ----- 普通链表 -----
        {
            List(i32) l1;
            Create(List(i32), &l1);
            for (int i = 0; i < N; i++)
                Call(List(i32), &l1, PushBack, i);

            ListIter(i32) it = _List_i32_Begin(&l1);
            BENCH_N("List seq random insert", EXTRA, {
                    for (int iter = 0; iter < EXTRA; iter++)
                    {
                        // 移动随机步数（只前进，可能绕回）
                        int step = steps[iter];
                        ListIter(i32) end = Call(List(i32), &l1, End);
                        for (int s = 0; s < step; s++)
                        {
                            Call(ListIter(i32), &it, Next);
                            if (Call(ListIter(i32), &it, Equals, &end))
                            {
                                it = Call(List(i32), &l1, Begin);
                            }
                        }
                        Call(List(i32), &l1, InsertAfter, it, iter);
                    }
                });
            Call(List(i32), &l1, Destroy);
        }

        // ----- PoolList -----
        {
            PList(i32) l2;
            Create(PList(i32), &l2);
            for (int i = 0; i < N; i++)
                Call(PList(i32), &l2, PushBack, i);

            PListIter(i32) it = Call(PList(i32), &l2, Begin);
            BENCH_N("PoolList seq random insert", EXTRA, {
                    for (int iter = 0; iter < EXTRA; iter++)
                    {
                        int step = steps[iter];
                        PListIter(i32) end = Call(PList(i32), &l2, End);
                        for (int s = 0; s < step; s++)
                        {
                            Call(PListIter(i32), &it, Next);
                            if (Call(PListIter(i32), &it, Equals, &end))
                            {
                                it = Call(PList(i32), &l2, Begin);
                            }
                        }
                        Call(PList(i32), &l2, InsertAfter, it, iter);
                    }
                });
            Call(PList(i32), &l2, Destroy);
        }

        free(steps);
    }
}

/* ============================================================
 * PoolDoublyList vs DoublyList（双向链表）
 * ============================================================ */

static void bench_doubly(void)
{
    printf("\n========== Doubly-Linked: DList  vs  PDList ==========\n");

    const int N = 200000;
    const int C = 50000;

    printf("\n--- PushBack %d ---\n", N);
    BENCH_N("DList PushBack", N, {
        DList(i32) l;
        Create(DList(i32), &l);
        for (int i = 0; i < N; i++) Call(DList(i32), &l, PushBack, i);
        Call(DList(i32), &l, Destroy);
        });
    BENCH_N("PDList PushBack", N, {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (int i = 0; i < N; i++) Call(PDList(i32), &l, PushBack, i);
        Call(PDList(i32), &l, Destroy);
        });

    printf("\n--- PushFront %d ---\n", N);
    BENCH_N("DList PushFront", N, {
        DList(i32) l;
        Create(DList(i32), &l);
        for (int i = 0; i < N; i++) Call(DList(i32), &l, PushFront, i);
        Call(DList(i32), &l, Destroy);
        });
    BENCH_N("PDList PushFront", N, {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (int i = 0; i < N; i++) Call(PDList(i32), &l, PushFront, i);
        Call(PDList(i32), &l, Destroy);
        });

    printf("\n--- Foreach %d (x5 rounds) ---\n", N);
    {
        DList(i32) l1;
        Create(DList(i32), &l1);
        for (int i = 0; i < N; i++) Call(DList(i32), &l1, PushBack, i);
        volatile i32 s1 = 0;
        BENCH_N("DList Foreach x5", 5, {
            for (int r = 0; r < 5; r++)
                foreach(DList(i32), v, l1) s1 += v;
            });
        Call(DList(i32), &l1, Destroy);

        PDList(i32) l2;
        Create(PDList(i32), &l2);
        for (int i = 0; i < N; i++) Call(PDList(i32), &l2, PushBack, i);
        volatile i32 s2 = 0;
        BENCH_N("PDList Foreach x5", 5, {
            for (int r = 0; r < 5; r++)
                foreach(PDList(i32), v, l2) s2 += v;
            });
        Call(PDList(i32), &l2, Destroy);
    }

    /* 槽位复用 */
    {
        int total_ops = C * 100;
        printf("\n--- Slot reuse: push50 + pop50 x %d ---\n", C);
        BENCH_N("DList slot reuse", total_ops, {
                DList(i32) l;
                Create(DList(i32), &l);
                for (int iter = 0; iter < C; iter++)
                {
                for (int j = 0; j < 50; j++) Call(DList(i32), &l, PushBack, j);
                for (int j = 0; j < 50; j++) Call(DList(i32), &l, PopFront);
                }
                Call(DList(i32), &l, Destroy);
            });
        BENCH_N("PDList slot reuse", total_ops, {
                PDList(i32) l;
                Create(PDList(i32), &l);
                for (int iter = 0; iter < C; iter++)
                {
                for (int j = 0; j < 50; j++) Call(PDList(i32), &l, PushBack, j);
                for (int j = 0; j < 50; j++) Call(PDList(i32), &l, PopFront);
                }
                Call(PDList(i32), &l, Destroy);
            });
    }

    /* Create/Destroy */
    {
        int iter = 5000;
        printf("\n--- Create/Destroy x %d (push 100 then destroy) ---\n", iter);
        BENCH_N("DList C/D", iter * 100, {
                for (int k = 0; k < iter; k++)
                {
                DList(i32) l;
                Create(DList(i32), &l);
                for (int i = 0; i < 100; i++) Call(DList(i32), &l, PushBack, i);
                Call(DList(i32), &l, Destroy);
                }
            });
        BENCH_N("PDList C/D", iter * 100, {
                for (int k = 0; k < iter; k++)
                {
                PDList(i32) l;
                Create(PDList(i32), &l);
                for (int i = 0; i < 100; i++) Call(PDList(i32), &l, PushBack, i);
                Call(PDList(i32), &l, Destroy);
                }
            });
    }

    printf("\n--- FIFO: push %d then pop all ---\n", N);
    BENCH_N("DList FIFO", N, {
        DList(i32) l;
        Create(DList(i32), &l);
        for (int i = 0; i < N; i++) Call(DList(i32), &l, PushBack, i);
        while (!Call(DList(i32), &l, IsEmpty))
            Call(DList(i32), &l, PopFront);
        Call(DList(i32), &l, Destroy);
        });
    BENCH_N("PDList FIFO", N, {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (int i = 0; i < N; i++) Call(PDList(i32), &l, PushBack, i);
        while (!Call(PDList(i32), &l, IsEmpty))
            Call(PDList(i32), &l, PopFront);
        Call(PDList(i32), &l, Destroy);
        });

    printf("\n--- LIFO: push %d then PopBack all ---\n", N);
    BENCH_N("DList LIFO", N, {
        DList(i32) l;
        Create(DList(i32), &l);
        for (int i = 0; i < N; i++) Call(DList(i32), &l, PushBack, i);
        while (!Call(DList(i32), &l, IsEmpty))
            Call(DList(i32), &l, PopBack);
        Call(DList(i32), &l, Destroy);
        });
    BENCH_N("PDList LIFO", N, {
        PDList(i32) l;
        Create(PDList(i32), &l);
        for (int i = 0; i < N; i++) Call(PDList(i32), &l, PushBack, i);
        while (!Call(PDList(i32), &l, IsEmpty))
            Call(PDList(i32), &l, PopBack);
        Call(PDList(i32), &l, Destroy);
        });

    printf("\n--- Reverse %d ---\n", N);
    {
        DList(i32) l1;
        Create(DList(i32), &l1);
        for (int i = 0; i < N; i++) Call(DList(i32), &l1, PushBack, i);
        BENCH("DList Reverse", { Call(DList(i32), &l1, Reverse); });
        Call(DList(i32), &l1, Destroy);

        PDList(i32) l2;
        Create(PDList(i32), &l2);
        for (int i = 0; i < N; i++) Call(PDList(i32), &l2, PushBack, i);
        BENCH("PDList Reverse", { Call(PDList(i32), &l2, Reverse); });
        Call(PDList(i32), &l2, Destroy);
    }

    /* ---- 双向链表：顺序推进随机插入（基于上一次迭代器，只前进）---- */
    {
        const int EXTRA = 2000;

        printf("\n--- Doubly sequential random insert: build %d, insert %d, move forward random steps ---\n", N, EXTRA);

        int* steps = malloc(EXTRA * sizeof(int));
        srand(12345);
        for (int i = 0; i < EXTRA; i++)
        {
            steps[i] = (rand() % 50) + 1;
        }

        // ----- DList（普通双链表）-----
        {
            DList(i32) l1;
            Create(DList(i32), &l1);
            for (int i = 0; i < N; i++)
                Call(DList(i32), &l1, PushBack, i);

            DListIter(i32) it = Call(DList(i32), &l1, Begin);
            BENCH_N("DList seq random insert", EXTRA, {
                    for (int iter = 0; iter < EXTRA; iter++)
                    {
                    int step = steps[iter];
                    for (int s = 0; s < step; s++)
                    {
                    Call(DListIter(i32), &it, Next);
                    DListIter(i32) end = Call(DList(i32), &l1, End);
                    if (Call(DListIter(i32), &it, Equals, &end))
                    {
                    it = Call(DList(i32), &l1, Begin);
                    }
                    }
                    // 在 it 之后插入：先取下一个位置，然后 Insert（之前）
                    DListIter(i32) next_it = it;
                    Call(DListIter(i32), &next_it, Next);
                    Call(DList(i32), &l1, Insert, next_it, iter);
                    }
                });
            Call(DList(i32), &l1, Destroy);
        }

        // ----- PDList（池化双链表）-----
        {
            PDList(i32) l2;
            Create(PDList(i32), &l2);
            for (int i = 0; i < N; i++)
                Call(PDList(i32), &l2, PushBack, i);

            PDListIter(i32) it = Call(PDList(i32), &l2, Begin);
            BENCH_N("PDList seq random insert", EXTRA, {
                    for (int iter = 0; iter < EXTRA; iter++)
                    {
                    int step = steps[iter];
                    for (int s = 0; s < step; s++)
                    {
                    Call(PDListIter(i32), &it, Next);
                    PDListIter(i32) end = Call(PDList(i32), &l2, End);
                    if (Call(PDListIter(i32), &it, Equals, &end))
                    {
                    it = Call(PDList(i32), &l2, Begin);
                    }
                    }
                    PDListIter(i32) next_it = it;
                    Call(PDListIter(i32), &next_it, Next);
                    Call(PDList(i32), &l2, Insert, next_it, iter);
                    }
                });
            Call(PDList(i32), &l2, Destroy);
        }

        free(steps);
    }
}

/* ============================================================
 * 混合场景："连续数据 + 较少插入"
 * 建一个大列表后做少量插入/删除，对比遍历性能
 * ============================================================ */

static void bench_mixed(void)
{
    printf("\n========== Mixed: small insertions on large list ==========\n");

    const int BASE = 200000;
    const int EXTRA = 1000;

    printf("\n--- Build %d, then PushFront %d ---\n", BASE, EXTRA);
    {
        List(i32) l1;
        Create(List(i32), &l1);
        for (int i = 0; i < BASE; i++) Call(List(i32), &l1, PushBack, i);
        BENCH_N("List PushFront x1000", EXTRA, {
            for (int i = 0; i < EXTRA; i++) Call(List(i32), &l1, PushFront, i);
            });
        Call(List(i32), &l1, Destroy);

        PList(i32) l2;
        Create(PList(i32), &l2);
        for (int i = 0; i < BASE; i++) Call(PList(i32), &l2, PushBack, i);
        BENCH_N("PoolList PushFront x1000", EXTRA, {
            for (int i = 0; i < EXTRA; i++) Call(PList(i32), &l2, PushFront, i);
            });
        Call(PList(i32), &l2, Destroy);
    }

    printf("\n--- Build %d, then PopFront %d ---\n", BASE, EXTRA);
    {
        List(i32) l1;
        Create(List(i32), &l1);
        for (int i = 0; i < BASE; i++) Call(List(i32), &l1, PushBack, i);
        BENCH_N("List PopFront x1000", EXTRA, {
            for (int i = 0; i < EXTRA; i++) Call(List(i32), &l1, PopFront);
            });
        Call(List(i32), &l1, Destroy);

        PList(i32) l2;
        Create(PList(i32), &l2);
        for (int i = 0; i < BASE; i++) Call(PList(i32), &l2, PushBack, i);
        BENCH_N("PoolList PopFront x1000", EXTRA, {
            for (int i = 0; i < EXTRA; i++) Call(PList(i32), &l2, PopFront);
            });
        Call(PList(i32), &l2, Destroy);
    }
}

/* ============================================================
 * Main
 * ============================================================ */

void benchmark(void)
{
    printf("================================================\n");
    printf("  Tomy List vs PoolList Benchmark\n");
    printf("================================================\n");

    bench_singly();
    bench_doubly();
    bench_mixed();

    printf("\n================================================\n");
    printf("  Done.\n");
    printf("================================================\n");
}

#endif