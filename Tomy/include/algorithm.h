#pragma once
#include <string.h>
#include <stdbool.h>
#include "data_type/data_type.h"
#include "class/object_class.h"
#include "ustring.h"

// ============================================================
// Tomy 泛型算法库
//
// 所有算法操作在 T* 指针区间 [begin, end) 上，适用于 Vec 的连续存储。
// 排序采用 introsort（快速排序 + 堆排序兜底）。
//
// 每个函数接受可选的比较器 cmp(a, b)：
//   返回 <0 表示 a < b
//   返回  0 表示 a == b
//   返回 >0 表示 a > b
//   传 NULL 则使用该类型的默认比较器（POD 用 < >，String 用 strcmp）
//
// Vec 便捷宏：
//   algo_sort(v, cmp)        → algo_i32_sort(algo_begin(&v), algo_end(&v), cmp)
//   algo_begin(v), algo_end(v) → 从 Vec 提取 T* 区间
// ============================================================

// ============================================================
// X-Macro: 所有算法实现（不含默认比较器）
// ============================================================

#if 0

#define _ALGO_IMPL_ALGOS(T)                                                                        \
                                                                                                    \
/* ---- Swap ---- */                                                                                \
static inline void _algo_##T##_swap(T* restrict a, T* restrict b) {                                       \
    T _tmp = *a;                                                                                    \
    *a = *b;                                                                                        \
    *b = _tmp;                                                                                      \
}                                                                                                   \
                                                                                                    \
/* ---- Insertion Sort (for small subarrays, <= 16 elements) ---- */                                \
static inline void _algo_##T##_insertion_sort(                                                             \
    T* const begin, T* const end, int (*cmp)(const T*, const T*))                                  \
{                                                                                                   \
    const umax count = (umax)(end - begin);                                                         \
    if (count <= 1) return;                                                                        \
    for (umax i = 1; i < count; ++i) {                                                             \
        T _key = begin[i];                                                                          \
        umax j = i;                                                                                 \
        while (j > 0 && cmp(&begin[j - 1], &_key) > 0) {                                           \
            begin[j] = begin[j - 1];                                                                \
            --j;                                                                                    \
        }                                                                                           \
        begin[j] = _key;                                                                            \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* ---- Heap helpers ---- */                                                                        \
static inline void _algo_##T##_sift_down(                                                                 \
    T* const begin, umax start, umax end_idx,                                                      \
    int (*cmp)(const T*, const T*))                                                                 \
{                                                                                                   \
    umax root = start;                                                                             \
    while (root * 2 + 1 <= end_idx) {                                                              \
        const umax child = root * 2 + 1;                                                           \
        umax swp = root;                                                                           \
        if (cmp(&begin[swp], &begin[child]) < 0) swp = child;                                      \
        if (child + 1 <= end_idx &&                                                                \
            cmp(&begin[swp], &begin[child + 1]) < 0) swp = child + 1;                              \
        if (swp == root) return;                                                                   \
        _algo_##T##_swap(&begin[root], &begin[swp]);                                               \
        root = swp;                                                                                \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
static inline void _algo_##T##_heap_sort(                                                                 \
    T* const begin, T* const end, int (*cmp)(const T*, const T*))                                  \
{                                                                                                   \
    const umax count = (umax)(end - begin);                                                         \
    if (count <= 1) return;                                                                        \
    const umax last = count - 1;                                                                   \
    for (umax i = count / 2; i > 0; --i)                                                           \
        _algo_##T##_sift_down(begin, i - 1, last, cmp);                                            \
    for (umax i = last; i > 0; --i) {                                                              \
        _algo_##T##_swap(&begin[0], &begin[i]);                                                    \
        _algo_##T##_sift_down(begin, 0, i - 1, cmp);                                              \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* ---- Partition: median-of-3 + Lomuto ---- */                                                     \
static inline void _algo_##T##_median_of_3(                                                               \
    T* const begin, T* const end, int (*cmp)(const T*, const T*))                                  \
{                                                                                                   \
    T* mid = begin + (end - begin) / 2;                                                            \
    T* last = end - 1;                                                                             \
    if (cmp(mid, begin) < 0)  _algo_##T##_swap(mid, begin);                                        \
    if (cmp(last, begin) < 0) _algo_##T##_swap(last, begin);                                       \
    if (cmp(last, mid) < 0)   _algo_##T##_swap(last, mid);                                         \
}                                                                                                   \
                                                                                                    \
static inline T* _algo_##T##_partition(                                                                   \
    T* const begin, T* const end, int (*cmp)(const T*, const T*))                                  \
{                                                                                                   \
    _algo_##T##_median_of_3(begin, end, cmp);                                                      \
    T* const pivot = end - 1;                                                                      \
    _algo_##T##_swap(begin + (end - begin) / 2, pivot);                                            \
    T* i = begin;                                                                                  \
    for (T* j = begin; j < pivot; ++j) {                                                           \
        if (cmp(j, pivot) < 0) {                                                                   \
            _algo_##T##_swap(i, j);                                                                \
            ++i;                                                                                   \
        }                                                                                          \
    }                                                                                               \
    _algo_##T##_swap(i, pivot);                                                                    \
    return i;                                                                                       \
}                                                                                                   \
                                                                                                    \
/* ---- Introsort core ---- */                                                                      \
static inline void _algo_##T##_introsort(                                                                 \
    T* const begin, T* const end, int (*cmp)(const T*, const T*),                                  \
    umax depth_limit)                                                                              \
{                                                                                                   \
    const umax count = (umax)(end - begin);                                                         \
    if (count <= 16) {                                                                             \
        _algo_##T##_insertion_sort(begin, end, cmp);                                               \
        return;                                                                                    \
    }                                                                                               \
    if (depth_limit == 0) {                                                                        \
        _algo_##T##_heap_sort(begin, end, cmp);                                                    \
        return;                                                                                    \
    }                                                                                               \
    T* pivot = _algo_##T##_partition(begin, end, cmp);                                             \
    _algo_##T##_introsort(begin, pivot, cmp, depth_limit - 1);                                     \
    _algo_##T##_introsort(pivot + 1, end, cmp, depth_limit - 1);                                   \
}                                                                                                   \
                                                                                                    \
/* ========== Public API ========== */                                                              \
                                                                                                    \
/* ---- sort ---- */                                                                                \
static inline void algo_##T##_sort(T* const begin, T* const end,                                          \
    int (*cmp)(const T*, const T*))                                                                 \
{                                                                                                   \
    const umax count = (umax)(end - begin);                                                         \
    if (count <= 1) return;                                                                        \
    umax depth = 0;                                                                                \
    for (umax n = count; n > 0; n >>= 1) ++depth;                                                  \
    depth *= 2;                                                                                    \
    if (!cmp) cmp = _algo_##T##_cmp_default;                                                        \
    _algo_##T##_introsort(begin, end, cmp, depth);                                                 \
}                                                                                                   \
                                                                                                    \
/* ---- lower_bound (first not less than value) ---- */                                             \
static inline T* algo_##T##_lower_bound(T* begin, T* const end,                                            \
    const T* value, int (*cmp)(const T*, const T*))                                                \
{                                                                                                   \
    if (!cmp) cmp = _algo_##T##_cmp_default;                                                        \
    umax count = (umax)(end - begin);                                                              \
    while (count > 0) {                                                                            \
        const umax step = count / 2;                                                               \
        T* it = begin + step;                                                                      \
        if (cmp(it, value) < 0) {                                                                  \
            begin = it + 1;                                                                        \
            count -= step + 1;                                                                     \
        } else {                                                                                   \
            count = step;                                                                          \
        }                                                                                          \
    }                                                                                               \
    return begin;                                                                                  \
}                                                                                                   \
                                                                                                    \
/* ---- upper_bound (first greater than value) ---- */                                              \
static inline T* algo_##T##_upper_bound(T* begin, T* const end,                                            \
    const T* value, int (*cmp)(const T*, const T*))                                                \
{                                                                                                   \
    if (!cmp) cmp = _algo_##T##_cmp_default;                                                        \
    umax count = (umax)(end - begin);                                                              \
    while (count > 0) {                                                                            \
        const umax step = count / 2;                                                               \
        T* it = begin + step;                                                                      \
        if (cmp(value, it) >= 0) {                                                                 \
            begin = it + 1;                                                                        \
            count -= step + 1;                                                                     \
        } else {                                                                                   \
            count = step;                                                                          \
        }                                                                                          \
    }                                                                                               \
    return begin;                                                                                  \
}                                                                                                   \
                                                                                                    \
/* ---- binary_search (requires sorted range) ---- */                                               \
static inline bool algo_##T##_binary_search(T* begin, T* const end,                                        \
    const T* value, int (*cmp)(const T*, const T*))                                                \
{                                                                                                   \
    if (!cmp) cmp = _algo_##T##_cmp_default;                                                        \
    T* it = algo_##T##_lower_bound(begin, end, value, cmp);                                        \
    return it != end && cmp(it, value) == 0;                                                       \
}                                                                                                   \
                                                                                                    \
/* ---- find ---- */                                                                                \
static inline T* algo_##T##_find(T* begin, T* const end,                                                   \
    const T* value, int (*cmp)(const T*, const T*))                                                \
{                                                                                                   \
    if (!cmp) cmp = _algo_##T##_cmp_default;                                                        \
    for (T* p = begin; p != end; ++p)                                                              \
        if (cmp(p, value) == 0) return p;                                                          \
    return end;                                                                                    \
}                                                                                                   \
                                                                                                    \
static inline T* algo_##T##_find_if(T* begin, T* const end, bool (*pred)(const T*))                       \
{                                                                                                   \
    for (T* p = begin; p != end; ++p)                                                              \
        if (pred(p)) return p;                                                                     \
    return end;                                                                                    \
}                                                                                                   \
                                                                                                    \
/* ---- count ---- */                                                                               \
static inline umax algo_##T##_count(T* begin, T* const end,                                                \
    const T* value, int (*cmp)(const T*, const T*))                                                \
{                                                                                                   \
    if (!cmp) cmp = _algo_##T##_cmp_default;                                                        \
    umax n = 0;                                                                                    \
    for (T* p = begin; p != end; ++p)                                                              \
        if (cmp(p, value) == 0) ++n;                                                               \
    return n;                                                                                      \
}                                                                                                   \
                                                                                                    \
static inline umax algo_##T##_count_if(T* begin, T* const end, bool (*pred)(const T*))                    \
{                                                                                                   \
    umax n = 0;                                                                                    \
    for (T* p = begin; p != end; ++p)                                                              \
        if (pred(p)) ++n;                                                                          \
    return n;                                                                                      \
}                                                                                                   \
                                                                                                    \
/* ---- fill ---- */                                                                                \
static inline void algo_##T##_fill(T* begin, T* const end, const T* value)                                \
{                                                                                                   \
    for (T* p = begin; p != end; ++p) *p = *value;                                                 \
}                                                                                                   \
                                                                                                    \
/* ---- reverse ---- */                                                                             \
static inline void algo_##T##_reverse(T* begin, T* const end)                                             \
{                                                                                                   \
    T* left = begin;                                                                               \
    T* right = end - 1;                                                                            \
    while (left < right) {                                                                         \
        _algo_##T##_swap(left, right);                                                             \
        ++left;                                                                                    \
        --right;                                                                                   \
    }                                                                                               \
}                                                                                                   \
                                                                                                    \
/* ---- min / max element ---- */                                                                   \
static inline T* algo_##T##_min_element(T* begin, T* const end,                                            \
    int (*cmp)(const T*, const T*))                                                                \
{                                                                                                   \
    if (begin == end) return end;                                                                  \
    if (!cmp) cmp = _algo_##T##_cmp_default;                                                        \
    T* result = begin;                                                                             \
    for (T* p = begin + 1; p != end; ++p)                                                          \
        if (cmp(p, result) < 0) result = p;                                                        \
    return result;                                                                                 \
}                                                                                                   \
                                                                                                    \
static inline T* algo_##T##_max_element(T* begin, T* const end,                                            \
    int (*cmp)(const T*, const T*))                                                                \
{                                                                                                   \
    if (begin == end) return end;                                                                  \
    if (!cmp) cmp = _algo_##T##_cmp_default;                                                        \
    T* result = begin;                                                                             \
    for (T* p = begin + 1; p != end; ++p)                                                          \
        if (cmp(p, result) > 0) result = p;                                                        \
    return result;                                                                                 \
}                                                                                                   \
                                                                                                    \
/* ---- all_of / any_of / none_of ---- */                                                           \
static inline bool algo_##T##_all_of(const T* begin, const T* end, bool (*pred)(const T*))                \
{                                                                                                   \
    for (const T* p = begin; p != end; ++p)                                                        \
        if (!pred(p)) return false;                                                                \
    return true;                                                                                   \
}                                                                                                   \
                                                                                                    \
static inline bool algo_##T##_any_of(const T* begin, const T* end, bool (*pred)(const T*))                \
{                                                                                                   \
    for (const T* p = begin; p != end; ++p)                                                        \
        if (pred(p)) return true;                                                                  \
    return false;                                                                                  \
}                                                                                                   \
                                                                                                    \
static inline bool algo_##T##_none_of(const T* begin, const T* end, bool (*pred)(const T*))               \
{                                                                                                   \
    for (const T* p = begin; p != end; ++p)                                                        \
        if (pred(p)) return false;                                                                 \
    return true;                                                                                   \
}                                                                                                   \
                                                                                                    \
/* ---- for_each ---- */                                                                            \
static inline void algo_##T##_for_each(T* begin, T* const end, void (*func)(T*))                          \
{                                                                                                   \
    for (T* p = begin; p != end; ++p) func(p);                                                     \
}

// ============================================================
// 默认比较器生成宏
// ============================================================

// --- POD 类型：使用 < > 运算符 ---
#define _ALGO_IMPL_CMP_DEFAULT(T) \
static inline int _algo_##T##_cmp_default(const T* a, const T* b) { \
    return (*a > *b) - (*a < *b); \
}

// ============================================================
// Vec 便捷宏
//
// 用法:
//   Vec(i32) v;
//   // ... 填充 v ...
//   algo_sort(&v, NULL);                   // 默认升序
//   algo_sort(&v, my_cmp);                 // 自定义比较器
//   i32 key = 42;
//   i32* p = algo_find(&v, &key, NULL);    // 查找
//   algo_reverse(&v);                      // 反转
//
// 也可以直接使用 typed 函数 + algo_begin/algo_end:
//   algo_sort_i32(algo_begin(&v), algo_end(&v), NULL);
// ============================================================

// --- 从 Vec 指针提取 T* begin / end ---
#define algo_begin(v)  ((_TYPE_OF((v)->front))((_VectorBase*)(v))->data)
#define algo_end(v)    (algo_begin(v) + ((_VectorBase*)(v))->size)

// --- _Generic 类型分发表（各 Vector 类型为独立 struct，无类型别名冲突） ---
#define _ALGO_CASES(name, b, e, ...) \
    Vector_i8:     name##_i8(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_byte:   name##_byte(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_i16:    name##_i16(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_i32:    name##_i32(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_i64:    name##_i64(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_imax:   name##_imax(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_u8:     name##_u8(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_u16:    name##_u16(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_u32:    name##_u32(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_u64:    name##_u64(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_umax:   name##_umax(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_f32:    name##_f32(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_f64:    name##_f64(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_Object: name##_Object(b, e __VA_OPT__(,) __VA_ARGS__), \
    Vector_String: name##_String(b, e __VA_OPT__(,) __VA_ARGS__)

#define algo_sort(v, cmp)          _Generic(*(v), _ALGO_CASES(algo_sort, algo_begin(v), algo_end(v), cmp))
#define algo_reverse(v)            _Generic(*(v), _ALGO_CASES(algo_reverse, algo_begin(v), algo_end(v)))
#define algo_lower_bound(v,vl,cmp) _Generic(*(v), _ALGO_CASES(algo_lower_bound, algo_begin(v), algo_end(v), vl, cmp))
#define algo_upper_bound(v,vl,cmp) _Generic(*(v), _ALGO_CASES(algo_upper_bound, algo_begin(v), algo_end(v), vl, cmp))
#define algo_binary_search(v,vl,cmp) _Generic(*(v), _ALGO_CASES(algo_binary_search, algo_begin(v), algo_end(v), vl, cmp))
#define algo_find(v,val,cmp)       _Generic(*(v), _ALGO_CASES(algo_find, algo_begin(v), algo_end(v), val, cmp))
#define algo_find_if(v,pred)       _Generic(*(v), _ALGO_CASES(algo_find_if, algo_begin(v), algo_end(v), pred))
#define algo_count(v,val,cmp)      _Generic(*(v), _ALGO_CASES(algo_count, algo_begin(v), algo_end(v), val, cmp))
#define algo_count_if(v,pred)      _Generic(*(v), _ALGO_CASES(algo_count_if, algo_begin(v), algo_end(v), pred))
#define algo_fill(v,val)           _Generic(*(v), _ALGO_CASES(algo_fill, algo_begin(v), algo_end(v), val))
#define algo_min_element(v,cmp)    _Generic(*(v), _ALGO_CASES(algo_min_element, algo_begin(v), algo_end(v), cmp))
#define algo_max_element(v,cmp)    _Generic(*(v), _ALGO_CASES(algo_max_element, algo_begin(v), algo_end(v), cmp))
#define algo_all_of(v,pred)        _Generic(*(v), _ALGO_CASES(algo_all_of, algo_begin(v), algo_end(v), pred))
#define algo_any_of(v,pred)        _Generic(*(v), _ALGO_CASES(algo_any_of, algo_begin(v), algo_end(v), pred))
#define algo_none_of(v,pred)       _Generic(*(v), _ALGO_CASES(algo_none_of, algo_begin(v), algo_end(v), pred))
#define algo_for_each(v,func)      _Generic(*(v), _ALGO_CASES(algo_for_each, algo_begin(v), algo_end(v), func))

#endif

// --- Object：比较地址 ---
static inline int _algo_Object_cmp_default(const Object* a, const Object* b)
{
    if (a == b) return 0;
    return ((uintptr_t)a < (uintptr_t)b) ? -1 : 1;
}

// --- String：比较字符串内容 ---
static inline int _algo_String_cmp_default(const String* a, const String* b)
{
    if (a == b) return 0;
    if (a->data == NULL && b->data == NULL) return 0;
    if (a->data == NULL) return -1;
    if (b->data == NULL) return 1;
    return strcmp(a->data, b->data);
}

static inline int _String_cmp_default(const String* a, const String* b)
{
    return _algo_String_cmp_default(a, b);
}

static inline int _Object_cmp_default(const Object* a, const Object* b)
{
    return _algo_Object_cmp_default(a, b);
}

#define SORT(containerType, container) \
    _SORT(containerType, container, NULL)
#define SORT_CMP(ContainerType, container, cmp) \
    _SORT(ContainerType, container, cmp)
#define _SORT(CT, container, cmp) \
    _##CT##_Sort(container, cmp)