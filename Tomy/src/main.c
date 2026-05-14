#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"
#include "test_config.h"

// 函数声明
#ifdef PRINT_ANY_TEST
void print_any_test(void);
#endif
#ifdef PRINT_FMT_TEST
void print_fmt_test(void);
#endif
#ifdef PRINT_COLOR_TEST
void print_color_test(void);
#endif
#ifdef PRINT_256_COLOR_TABLE
void print_256_color_table(void);
#endif
#ifdef TEST_1
void test_1(void);
#endif
#ifdef TEST_2
void test_2(void);
#endif
#ifdef TEST_3
void test_3(void);
#endif
#ifdef VECTOR_TEST
void vector_test(void);
#endif
#ifdef HASHMAP_TEST
void hashmap_test(void);
#endif
#ifdef LIST_TEST
void list_test(void);
#endif
#ifdef DLIST_TEST
void dlist_test(void);
#endif
#ifdef POOLLIST_TEST
void pool_list_test(void);
#endif
#ifdef POOLDList_TEST
void pool_dlist_test(void);
#endif
#ifdef ALGORITHM_TEST
void algorithm_test(void);
#endif
#ifdef BENCHMARK
void benchmark(void);
#endif
#ifdef LIFETIME_TEST
void lifetime_test(void);
#endif

void test(void)
{
#ifdef PRINT_ANY_TEST
    print_any_test();
#endif
#ifdef PRINT_FMT_TEST
    print_fmt_test();
#endif
#ifdef PRINT_COLOR_TEST
    print_256_color_table();
#endif
#ifdef TEST_1
    test_1();
#endif
#ifdef TEST_2
    test_2();
#endif
#ifdef TEST_3
    test_3();
#endif
#ifdef VECTOR_TEST
    vector_test();
#endif
#ifdef HASHMAP_TEST
    hashmap_test();
#endif
#ifdef LIST_TEST
    list_test();
#endif
#ifdef DLIST_TEST
    dlist_test();
#endif
#ifdef POOLLIST_TEST
    pool_list_test();
#endif
#ifdef POOLDList_TEST
    pool_dlist_test();
#endif
#ifdef ALGORITHM_TEST
    algorithm_test();
#endif
#ifdef BENCHMARK
    benchmark();
#endif
#ifdef LIFETIME_TEST
    lifetime_test();
#endif
}

int main(int argc, char** argv)
{
    test();
    return 0;
}
