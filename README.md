# Tomy — C 语言增强库

> 纯 C 实现的泛型容器库 + OOP 宏体系 + 类型安全打印系统。零外部依赖。

Tomy 在标准 C 之上构建了一个轻量的面向对象框架，提供**泛型容器**（Vector、List、DoublyList、PoolList、HashMap）、**继承与虚函数调度**、**类型泛化打印**等能力——全部通过宏和内联函数实现，无需 C++ 编译器。

---

## 快速开始

### 构建

```bash
# 命令行构建
msbuild Tomy.sln /p:Configuration=Debug /p:Platform=x64

# 或在 Visual Studio 中打开 Tomy.sln 直接编译运行
```

### 最小示例

```c
#include "tomy.h"

int main(void)
{
    /* Vec(f32) — 泛型动态数组 */
    Vec(f32) v;
    Create(Vec(f32), &v);

    FT(Vec(f32), &v)->push_back(&v, 1.0f);
    FT(Vec(f32), &v)->push_back(&v, 3.0f);
    FT(Vec(f32), &v)->push_back(&v, 2.0f);

    /* foreach 迭代 */
    foreach (Vec(f32), val, v) {
        println(val);
    }

    /* 排序 */
    SORT(Vec(f32), &v);

    v.vptr->destroy(&v);
    return 0;
}
```

### 示例代码

`examples/` 目录下为每个容器和算法提供了完整的独立示例：

| 文件 | 内容 |
|------|------|
| `oop_example.c` | 自定义类继承 Animal→Cat/Dog，虚函数重写，多态派发，VCall/FT 等效演示 |
| `vector_example.c` | Vector push_back/at/foreach/sort/reserve/emplace |
| `list_example.c` | 单链表 PushFront/InsertAfter/BeforeBegin/Reverse/Sort |
| `doublylist_example.c` | 双向链表 PopBack/Insert/Erase/Reverse/Sort |
| `pool_list_example.c` | 内存池化链表 PushBack/自动缩容/Iterator |
| `hashmap_example.c` | HashMap Insert/At/Erase/Find/TryEmplace/foreach |
| `algorithm_example.c` | Vec/List/DList 排序 + 反转 + 迭代器遍历 |

---

## 核心特性

### 1. OOP 宏体系 — 类继承与虚函数调度

所有容器（Vec、List、DList 等）和自定义类均**继承自 `Object`**，通过 vptr 实现运行时多态。`VCall` 和 `FT` 在调用函数上作用一致，只是写法不同：

- `VCall(T, self, func, ...)` — 通过 vptr 一次调用
- `FT(T, self)->func(self, ...)` — 先取 vtable 指针再调用

```c
/* ===== 定义类 ===== */

VTABLE {
    FROM(_Object_VTable);          /* 继承 Object 虚表（含 Create/Destroy/New/Delete/To_string） */
    int  (*get_value)(void* self);
    void (*set_value)(void* self, int v);
} _Counter_VTable;

CLASS {
    FROM(Object);                   /* 继承 Object，嵌入 vptr */
    int count;
} Counter;

/* ===== 实现方法 ===== */

static int  _Counter_GetValue(void* self) { return ((Counter*)self)->count; }
static void _Counter_SetValue(void* self, int v) { ((Counter*)self)->count = v; }

/* ===== Create 中注册虚表 ===== */

inline void _Counter_Create(Counter* self) {
    static _Counter_VTable vt = {
        .create    = _Counter_Create,
        .destroy   = _Object_Destroy,
        .new_      = _Counter_New,
        .delete_   = _Counter_Delete,
        .to_string = _Counter_ToString,
        .get_value = _Counter_GetValue,
        .set_value = _Counter_SetValue,
    };
    Object_Create((Object*)self);
    self->vptr = &vt;               /* 重写 vptr 指向本类虚表 */
    self->count = 0;
}

/* ===== 使用 ===== */

Counter c;
Create(Counter, &c);                /* 初始化，设置 vptr */

/* VCll 等价于 FT，以下两种写法完全一致 */
VCall(Counter, &c, set_value, 42);  /* 通过宏单步调用 */
FT(Counter, &c)->set_value(&c, 99); /* 先取虚表再手动调用 */

int val = VCall(Counter, &c, get_value);
/* val == 99 */

/* 所有容器类均继承自 Object，所以 Vector 等也支持 vptr 操作 */
Vec(f32) v;
Create(Vec(f32), &v);
VCall(Vec(f32), &v, is_empty);      /* true */
FT(Vec(f32), &v)->push_back(&v, 1.0f);
v.vptr->destroy(&v);                /* vptr 等价于 FT(Vec(f32), &v) */
```

| 宏/函数 | 说明 |
|---------|------|
| `Create(T, self)` | 初始化对象，设置 vptr |
| `VCall(T, self, func, ...)` | 通过 vptr 运行时派发（自动解引用） |
| `FT(T, self)` | 获取虚表指针，`VCall` 等价于 `FT(T,self)->func(self,...)` |
| `Call(T, self, Func, ...)` | 静态直接调用 `_T_Func(self, ...)`（不经过 vptr） |
| `New(T, ...)` | 堆分配 + 初始化 |
| `Delete` / `Destroy` | 析构释放 |

### 2. 动态字符串 (String)

自动扩容的字符串类型。String 结构体本身支持栈或堆分配，但其 **内部 `data` 缓冲区始终分配在堆上**，由 `capacity` / `size` 管理。

- `New(String, cap)` — 在堆上分配 `String` 结构体，同时初始化内部缓冲区
- `Create(String, &s)` — 在栈上初始化已存在的 `String`（内部 data 仍在堆上）
- `_String_Destroy(&s)` — 仅释放内部 data，不释放栈上的 String 结构体

```c
String* s = New(String, 32);       // 结构体 + 内部缓冲区均在堆上
Call(String, s, Append, "hello");
Call(String, s, Delete);           // 释放内部缓冲区 + 结构体自身

String ss;                          // 结构体在栈上
Create(String, &ss);                // 内部 data 在堆上分配
Call(String, &ss, Append, "world");
Call(String, &ss, Destroy);          // 释放内部 data，ss 本身不释放
```

### 3. 类型安全打印

通过 `_Generic` 实现类型感知打印，支持 `{}` 格式串和 ANSI 颜色。

```c
print(42, 3.14, "hello");      // 自动类型推断
println("{} is {}", "x", 42);  // 占位符替换
print(set_fg_rgb(255,0,0), "red", reset_style());
```

### 4. 错误检查

宏体系自动输出文件名和行号。

```c
ERR_RET_NULL(ptr);
ERR_RET_V_COND(index >= size, NULL);
ERR_RET_NULL_MSG(ptr, "custom message");
```

### 5. 宏工具

变参遍历、类型 ID、MIN/MAX、分支预测、内联控制等。

---

## 容器详解

### Vector — 泛型动态数组

连续内存存储，翻倍扩容。支持迭代器、foreach、排序（IntroSort）。

```c
Vec(f32) v;                           // 定义
Create(Vec(f32), &v);                 // 初始化
FT(Vec(f32), &v)->push_back(&v, 1.0f); // 尾部追加
f32* val = FT(Vec(f32), &v)->at(&v, 0); // 索引访问
SORT(Vec(f32), &v);                   // 排序
foreach (Vec(f32), x, v) { ... }      // 迭代
v.vptr->destroy(&v);                  // 销毁
```

| 方法 | 说明 |
|------|------|
| `push_back` / `pop_back` | 尾部操作 |
| `at(index)` | 索引访问 |
| `front` / `back` | 首尾元素 |
| `data` | 裸指针 |
| `reserve(n)` | 预分配容量 |
| `resize(n)` | 调整大小 |
| `insert(index, val)` | 插入 |
| `erase(index)` | 删除 |
| `emplace_back` | 原地构造 |
| `swap_erase(index)` | 交换删除（O(1)） |
| `shrink_to_fit` | 释放多余内存 |
| `Sort` | 排序（IntroSort） |

#### 元素生命周期回调

实例化 Vector 时可通过 `VECTOR_IMPL_EX(T, construct, destroy, copy, cmp)` 自定义元素的生命周期：

| 回调 | 语义 |
|------|------|
| `construct(void* addr)` | **构造**：在 `addr` 处初始化一个新元素（类似定位 new）。默认行为是 `memset` 清零。 |
| `destroy(void* addr)` | **析构**：销毁 `addr` 处的元素，释放其持有的资源。默认行为是空操作。 |
| `copy(void* dest, const void* src)` | **拷贝构造**：将 `src` 的内容拷贝到 `dest`。**`dest` 指向的内存处于未定义状态**，拷贝构造前不会调用析构 — 实现必须能直接覆盖目标内存。默认行为是 `memcpy`。 |

例如 `VECTOR_IMPL_EX(String, _String_Create, _String_Destroy, _String_Copy, _String_cmp_default)`。

POD 类型使用 `VECTOR_IMPL(T)` 快速实例化（`NULL` 回调 → `memset`/`memcpy`）。

已预实例化类型：i8/i16/i32/i64/imax + u8/byte/u16/u32/u64/umax + f32/f64 + Object + String。

---

### List — 单向链表

带头节点的单链表，每个节点独立 malloc。

```c
List(i32) l;
Create(List(i32), &l);
Call(List(i32), &l, PushBack, 10);
Call(List(i32), &l, PushFront, 5);
i32* f = Call(List(i32), &l, Front);

/* BeforeBegin 哨兵模式 */
ListIter(i32) bb = Call(List(i32), &l, BeforeBegin);
Call(List(i32), &l, InsertAfter, bb, 1);

SORT(List(i32), &l);  /* 归并排序 */
Call(List(i32), &l, Reverse);
l.vptr->destroy(&l);
```

| 方法 | 说明 |
|------|------|
| `PushFront` / `PopFront` | 头部操作 |
| `PushBack` | 尾部追加 |
| `Front` / `Back` | 首尾访问 |
| `InsertAfter(pos, val)` | 在迭代器之后插入 |
| `EraseAfter(pos)` | 删除迭代器之后的节点 |
| `BeforeBegin` | 哨兵迭代器（用于头部插入） |
| `Reverse` | 反转 |
| `Sort` | 归并排序 |
| `Clear` | 清空 |

---

### DoublyList — 双向链表

循环哨兵 sentinel 实现，O(1) 头和尾双向操作。

```c
DList(i32) l;
Create(DList(i32), &l);
Call(DList(i32), &l, PushBack, 10);
Call(DList(i32), &l, PushFront, 5);
Call(DList(i32), &l, PopBack);

DListIter(i32) it = VCall(DList(i32), &l, begin);
Call(DList(i32), &l, Insert, it, 7);   /* 在迭代器之前插入 */
Call(DList(i32), &l, Erase, it);       /* 删除迭代器指向的节点 */

SORT(DList(i32), &l);
```

| 方法 | 说明 |
|------|------|
| `PushFront` / `PopFront` | 头部操作 |
| `PushBack` / `PopBack` | 尾部操作 |
| `Insert(pos, val)` | 在迭代器之前插入 |
| `Erase(pos)` | 删除迭代器指向的节点 |
| `Reverse` | 反转 |
| `Sort` | 归并排序 |

---

### PoolList — 内存池化单向链表

连续内存块（slot 数组），节点用 `umax` 索引代替指针。自动扩容（2x）/ 缩容（<1/4）。

```c
PList(i32) l;
Create(PList(i32), &l);
Call(PList(i32), &l, PushBack, 10);
Call(PList(i32), &l, PushFront, 5);

/* BeforeBegin 哨兵模式（同 List） */
PListIter(i32) bb = Call(PList(i32), &l, BeforeBegin);
Call(PList(i32), &l, InsertAfter, bb, 1);

Call(PList(i32), &l, Destroy);
```

与 List 完全相同的 API，但底层由内存池管理，减少碎片化和 malloc 次数。

---

### HashMap — 哈希表

开放寻址 + 线性探测，负载因子 0.75。自动 rehash，支持墓碑标记。

```c
HashMap(i32, f64) m;
Create(HashMap(i32, f64), &m);
Call(HashMap(i32, f64), &m, Insert, 1, 10.5);
f64* v = Call(HashMap(i32, f64), &m, At, 1);
Call(HashMap(i32, f64), &m, Erase, 1);

/* foreach 返回 Pair 结构体 */
foreach (HashMap(i32, f64), p, m) {
    println(*p.key, "->", *p.value);
}

/* Find 返回迭代器 */
HashMapIter(i32, f64) it = Call(HashMap(i32, f64), &m, Find, 42);

m.vptr->destroy(&m);
```

| 方法 | 说明 |
|------|------|
| `Insert(key, val)` | 插入/覆盖 |
| `At(key)` | 查找（返回指针，NULL 表示不存在） |
| `Erase(key)` | 删除 |
| `Contains(key)` | 是否包含键 |
| `Find(key)` | 返回迭代器 |
| `TryEmplace(&key, &val)` | 不存在时插入 |
| `Rehash(n)` | 显式 rehash |
| `Reserve(n)` | 预分配容量 |
| `ShrinkToFit` | 缩容 |
| `LoadFactor` | 当前负载因子 |
| `Swap` | 交换两个 HashMap |

预实例化类型：`HashMap(i32, i32)`、`HashMap(i32, f64)`、`HashMap(u64, f64)`、`HashMap(String, i32)`、`HashMap(String, String)`。

---

## 算法

### 排序

使用 `SORT` 或 `SORT_CMP` 宏对容器进行排序。

```c
/* Vector 排序（IntroSort：快速排序 + 堆排序兜底） */
SORT(Vec(i32), &v);            /* 默认升序 */
SORT_CMP(Vec(i32), &v, cmp);   /* 自定义比较器 */

/* List / DList 排序（归并排序） */
SORT(List(i32), &l);
SORT(DList(i32), &dl);
```

比较器签名：
```c
int cmp(const T* a, const T* b);  /* 返回 <0, =0, >0 */
```

### 迭代器

所有容器均提供迭代器，支持 `foreach` 宏和手动遍历。

```c
/* foreach 自动迭代 */
foreach (Vec(f32), val, v) { print(val); }

/* 手动迭代器 */
VecIter(f32) it  = VCall(Vec(f32), &v, begin);
VecIter(f32) end = VCall(Vec(f32), &v, end);
while (!VCall(VecIter(f32), &it, equals, &end)) {
    f32 val = VCall(VecIter(f32), &it, get);
    VCall(VecIter(f32), &it, next);
}
```

| 迭代器方法 | 说明 |
|-----------|------|
| `get` | 获取当前值 |
| `key` / `value` | HashMap 迭代器专用 |
| `next` | 前进到下一个 |
| `equals` | 比较两个迭代器 |
| `raw` | 获取数据指针 |

每个容器的迭代器类型别名：

| 容器 | 迭代器别名 |
|------|-----------|
| `Vec(T)` | `VecIter(T)` |
| `List(T)` | `ListIter(T)` |
| `DList(T)` | `DListIter(T)` |
| `PList(T)` | `PListIter(T)` |
| `PDList(T)` | `PDListIter(T)` |
| `HashMap(K,V)` | `HashMapIter(K,V)` |

---

## 模块来源

| 模块 | 来源 | 说明 |
|------|------|------|
| OOP 宏体系（CLASS/VTABLE/VCall） | **HYTomZ 自写** | C 中实现类继承和虚函数调度 |
| Vector 容器 | **HYTomZ 自写** | 泛型动态数组，支持生命周期回调 |
| List / DoublyList | **HYTomZ 自写** | 单链表/双向链表 |
| PoolList / PoolDoublyList | **HYTomZ 自写** | 内存池化链表 |
| HashMap | **HYTomZ 自写** | 开放寻址哈希表 |
| 算法（SORT / SORT_CMP） | **HYTomZ 自写** | IntroSort + 归并排序 |
| 打印系统（print.h） | **emin** | 类型泛化打印、{} 格式化、ANSI 颜色 |
| 动态字符串（ustring.h/.c） | **emin** | 自动扩容 String 类型 |
| 错误处理（error.h） | 基于 emin + 自写 | 错误检查宏体系 |

---

## 构建说明

### Visual Studio

```
msbuild Tomy.sln /p:Configuration=Debug /p:Platform=x64
```

或直接在 VS 中打开 `Tomy.sln`。

### 启用测试

测试入口在 `src/main.c`，通过宏开关控制：

```c
#define VECTOR_TEST       // Vector 测试
#define HASHMAP_TEST      // HashMap 测试
#define LIST_TEST         // List 测试
#define DLIST_TEST        // DoublyList 测试
#define POOLLIST_TEST     // PoolList 测试
#define POOLDList_TEST    // PoolDoublyList 测试
#define ALGORITHM_TEST    // 算法测试
#define PRINT_ANY_TEST    // 打印测试
```

---

## 设计思路

Tomy 的设计深受 Godot Engine 的影响：

1. **虚表继承** — 类似 Godot 的 `Object` 体系和 `ClassDB`，每个类有自己的 vtable 实例，通过 `vptr` 实现运行时多态。
2. **类型泛化打印** — `print` 通过 `_Generic` 实现类型感知，类似 Rust 的 `Display` trait。
3. **生命周期管理** — Vector 支持构造/析构/拷贝回调，类似 C++ 的 allocator 模型。
4. **宏驱动代码生成** — X-Macro 模式为不同类型生成专用代码，无需运行时类型擦除。
5. **内存池化链表** — PoolList/PoolDoublyList 用 `umax` 索引取代指针，结合连续内存分配减少碎片。
