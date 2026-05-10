# Tomy — C 语言增强库

> 一个为 C 语言提供语法糖和 OOP 模式的**缝合增强库**，整合了多个优秀开源库的模块，加上自定义的 OOP 封装。

## 项目结构

```
Tomy/
├── Tomy.sln                 # Visual Studio 解决方案
├── Tomy/                    # 库源码
│   ├── Tomy.vcxproj         # VS 项目文件
│   ├── include/             # 头文件
│   └── src/                 # 源文件
└── README.md                # 本文件
```

### 头文件一览

| 路径 | 说明 |
|------|------|
| `include/tomy.h` | 主入口头文件，聚合所有模块 |
| `include/macro.h` | 类型 ID 枚举、变参宏（EXPAND/LAST/SELECT）、MIN/MAX 安全宏、foreach 迭代、内联控制 |
| `include/error.h` | 错误检查宏体系（空指针、条件、索引检查） |
| `include/print.h` | 格式化输出系统（类型泛化打印、`{}` 格式字符串、ANSI 颜色、流控制） |
| `include/ustring.h` | 动态字符串类型 `String` |
| `include/class/class_macro.h` | 面向对象宏体系（CLASS/VTABLE/VCall/Call） |
| `include/class/object_class.h` | 基类 `Object`，vtable 虚表模式 |
| `include/data_type/data_type.h` | 定长类型别名（i8~u64、f32/f64、byte、imax/umax） |
| `include/data_type/vector.h` | 泛型动态数组 `Vector`（X-Macro 生成 + OOP 虚表） |

---

## 模块来源

| 模块 | 来源 | 说明 |
|------|------|------|
| OOP 宏体系（CLASS/VTABLE/VCall） | **HYTomZ 自写** | 用宏在 C 中实现类继承和虚函数调度 |
| Vector 容器（`vector.h`） | **HYTomZ 自写** | 泛型动态数组，支持元素生命周期回调 |
| 打印系统（`print.h`） | **emin** | 类型泛化打印、`{}` 格式化、ANSI 颜色控制 |
| 动态字符串（`ustring.h/.c`） | **emin** | 自动扩容的 String 类型 |
| 错误处理（`error.h`） | 基于 emin + 自写 | 错误检查宏体系 |
| 宏工具（`macro.h`） | **HYTomZ 自写** | 变参遍历、类型 ID、MIN/MAX、内联控制 |
| 类型别名（`data_type.h`） | **HYTomZ 自写** | 定长类型别名 |

> 注：string 的命名风格和码风与其他模块有所不同，因为它来自 emin 的代码库。

### 1. OOP 宏体系 (`class/`)

通过宏在 C 中实现类似 C++ 的类继承和虚函数调度。

#### 定义类
```c
VTABLE {
    FROM(_Object_VTable);       // 继承基类虚表
    void (*foo)(void* self, int x);
} _MyClass_VTable;

CLASS {
    FROM(Object);               // 继承 Object
    int value;
} MyClass;
```

#### 虚函数调用
```c
// 通过虚表调用（运行时多态）
VCall(MyClass, &obj, foo, 42);

// 通过函数表直接调用
Call(MyClass, &obj, foo, 42);

// 直接获取虚表指针
FT(MyClass, &obj)->foo(&obj, 42);
```

#### 基类 Object
所有类的基类，提供统一的生命周期管理：

| 函数 | 说明 |
|------|------|
| `Object_Create(self)` | 初始化 vptr |
| `Object_New()` | 分配 + 初始化 |
| `_Object_Destroy(self)` | 清空 vptr |
| `_Object_Delete(self)` | 析构 + 释放 |
| `_Object_ToString(self)` | 返回 "Object" 字符串 |

---

### 2. 泛型动态数组 (`Vector`)

基于 `_VectorBase` 的泛型动态数组，通过 X-Macro `_VECTOR_IMPL_EX1` 生成类型化版本。

#### 使用方式
```c
// 定义并使用 float 向量
Vec(f32) vec;
Create(Vec(f32), &vec);

VCall(Vec(f32), &vec, push_back, 3.14f);
f32* val = VCall(Vec(f32), &vec, at, 0);

// foreach 迭代
foreach(Vec(f32), num, vec) {
    println_emin(num);
}

VCall(Vec(f32), &vec, destroy);
```

#### Vector 操作方法

| 方法 | 说明 |
|------|------|
| `push_back` | 尾部追加 |
| `pop_back` | 尾部移除 |
| `at(index)` | 索引访问 |
| `front` / `back` | 首尾元素 |
| `data` | 裸指针 |
| `resize(n)` | 调整大小（自动构造/析构） |
| `reserve(n)` | 预分配容量 |
| `clear` | 清空（容量保留） |
| `erase(index)` | 删除指定位置 |
| `is_empty` | 判空 |
| `begin` / `end` | 迭代器（配合 `foreach`） |

#### 元素生命周期
支持自定义构造/析构/拷贝回调：

```c
_VECTOR_IMPL_EX1(T, constructor_fn, destructor_fn, copy_fn)
```

若不提供回调，则使用 `memset`/`memcpy` 进行 POD 操作。

---

### 3. 动态字符串 (`String`)

自动扩容的字符串类型。

```c
typedef struct string_t {
    size_t capacity;   // 缓冲区容量
    size_t size;       // 实际长度
    char* data;        // 数据指针
} String;
```

| 函数 | 说明 |
|------|------|
| `_String_New(cap)` | 堆分配新字符串 |
| `_String_Delete(self)` | 释放字符串 |
| `_String_Create(self)` | 栈初始化（默认容量 32） |
| `_String_CreateN(self, cap)` | 指定容量初始化 |
| `_String_Destroy(self)` | 栈析构 |
| `_String_Append(str, s)` | 追加 C 字符串 |
| `_String_AppendN(str, s, n)` | 追加指定长度 |
| `_String_Insert(str, pos, s)` | 插入 C 字符串 |
| `_String_InsertN(str, pos, s, n)` | 插入指定长度 |
| `_String_Copy(dst, src)` | 拷贝 |
| `_String_Reserve(self, new_cap)` | 重新分配 |

扩容策略：按需翻倍（`calculate_capacity`）。

---

### 4. 打印系统 (`print.h`)

类型安全的泛型打印，支持 `printf` 格式串风格的 `{}` 格式化。

#### 基本打印
```c
// 自动检测类型打印
print_emin("hello", 42, 3.14);     // 输出: hello 42 3.14
println_emin("hello", 42, 3.14);   // 带换行
```

#### 格式化字符串
```c
// 使用 {} 占位
print_emin("{} is {}", "value", 42);
```

#### 颜色控制
```c
// 16 色索引
print_emin(set_fg_idx(COLOR_RED), "RED", reset_style(), set(sep=""));

// 24-bit RGB
print_emin(set_fg_rgb(102, 255, 178), "Custom", reset_style(), set(sep=""));
print_emin(set_fg_color(rgb(255, 100, 50)), "Hello", reset_style(), set(sep=""));

// 背景色
print_emin(set_bg_idx(COLOR_BLUE), "BG", reset_style());
print_emin(set_bg_rgb(30, 30, 30), "Dark", reset_style());

// 光标控制
print_emin(set_cursor_pos(10, 5), "At position", set(end=""));
```

#### 颜色常量
| 常量 | 值 |
|------|-----|
| `COLOR_BLACK` ~ `COLOR_WHITE` | 0~7（标准色） |
| `COLOR_DARK_*` / `COLOR_BRIGHT_*` | 0~15（完整 16 色） |

#### 打印配置
```c
// 自定义分隔符、结束符、输出流
print_emin(set(sep=", ", end="!\n", file=stderr, flush=true), 1, 2, 3);
```

#### 辅助宏
| 宏 | 说明 |
|------|------|
| `format(fmt, ...)` | 返回 `String*` 格式化结果 |
| `set(...)` | 创建 `PrintConfig`（花括号初始化） |
| `rgb(r,g,b)` / `rgba(r,g,b,a)` | 构造 `Color24` |
| `reset_style()` | ANSI 重置序列 |
| `set_fg_idx(n)` / `set_bg_idx(n)` | 256 色索引前景/背景 |
| `set_fg_rgb(r,g,b)` / `set_bg_rgb(r,g,b)` | RGB 前景/背景 |
| `set_fg_color(c)` / `set_bg_color(c)` | Color24 前景/背景 |
| `set_colors_idx(fg,bg)` / `set_colors_rgb(...)` / `set_colors_color(...)` | 组合设置 |

---

### 5. 错误处理 (`error.h`)

基于宏的错误检查体系，自动输出调用位置。

| 宏 | 说明 |
|------|------|
| `ERR_RET_NULL(ptr)` | 指针为空则打印错误并 `return` |
| `ERR_RET_NULL_MSG(ptr, msg)` | 同上，带自定义消息 |
| `ERR_RET_V_NULL(ptr, retval)` | 指针为空则返回 `retval` |
| `ERR_RET_V_NULL_MSG(ptr, retval, msg)` | 同上，带消息 |
| `ERR_RET_V_COND(cond, retval)` | 条件为真则返回 `retval` |
| `ERR_RET_V_COND_MSG(cond, retval, msg)` | 同上，带消息 |

所有错误信息格式：
```
ERROR: <错误消息>
   at: <函数名> (<文件名>:<行号>)
```

---

### 6. 宏工具 (`macro.h`)

#### 变参遍历
```c
EXPAND(func, a, b, c)   // → func(a), func(b), func(c)
LAST(a, b, c)           // → c
COUNT_ARGS(a, b, c)     // → 3
ARGS_COUNT(...)         // 变参个数（含零参数）
```

#### 类型匹配
```c
typeid_of(x)    // 返回类型枚举值（TYPE_INT, TYPE_STRING...）
```

#### MIN/MAX
```c
MIN_SAFE(a, b)   // 类型安全，多一个求值（GCC 用 statement-expr，MSVC 用 _Generic）
MIN_FAST(a, b)   // 快速版，类型不安全
```

#### 编译控制
```c
likely(cond) / unlikely(cond)       // 分支预测提示
_ALWAYS_INLINE_ / _FORCE_INLINE_    // 强制内联
_NO_INLINE_                         // 禁止内联
_ALLOW_DISCARD_                     // (void) 丢弃返回值
```

#### 字符串化
```c
_STR(x)          // → "x"
FUNCTION_STR     // 当前函数名
```

---

### 7. 类型别名 (`data_type.h`)

```c
i8, i16, i32, i64      // 有符号定长整型
u8, u16, u32, u64      // 无符号定长整型
byte                   // u8 别名
imax, umax             // intmax_t / uintmax_t
f32, f64               // 浮点类型
```

---

## 构建

项目使用 Visual Studio 构建（`Tomy.sln` / `Tomy.vcxproj`）。

- 在 VS 中打开 `Tomy.sln` 直接编译运行
- 测试入口在 `src/main.c`，通过宏开关选择测试用例

---

## 设计思路

Tomy 的设计深受 Godot Engine 的影响：

1. **虚表继承** — 类似 Godot 的 `Object` 体系和 `ClassDB`，每个类有自己的 vtable 实例，通过 `vptr` 实现运行时多态。
2. **类型泛化打印** — `print_emin` 通过 `_Generic` 实现类型感知，类似 Rust 的 `Display` trait 或 C++ 的 `operator<<`。
3. **Vector 的生命周期管理** — 支持构造/析构回调，类似 C++ `std::vector` 的 allocator 模型。
4. **宏驱动代码生成** — `_VECTOR_IMPL_EX1` 是典型的 X-Macro 模式，为不同类型生成专用代码。
