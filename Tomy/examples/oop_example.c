/**
 * oop_example.c — OOP 宏体系：继承与多态示例
 *
 * 演示自定义类继承 Object、虚函数调度（VCall/FT）、
 * 栈分配（Create/Destroy）和堆分配（New/Delete）。
 *
 * 所有容器类（Vec、List、DList 等）均遵循完全相同的 Object 继承 + vptr 模式。
 *
 * 编译: cl oop_example.c /I../Tomy/include /Fe:oop_example.exe
 * 运行: ./oop_example
 */

#include "tomy.h"

/* ================================================================
 * 步骤 1: 定义基类 Animal
 * ================================================================
 * VTABLE 声明虚函数表结构体，FROM 继承 _Object_VTable。
 * CLASS 声明对象结构体，FROM 嵌入 Object。
 * 所有类必须继承 Object，通过 vptr 实现多态。
 */

VTABLE{
    FROM(_Object_VTable);
    void (*speak)(void* self);
    void (*walk)(void* self, int steps);
} _Animal_VTable;

CLASS{
    FROM(Object);
    const char* name;
} Animal;

/* ---- 方法声明 ---- */
void _Animal_Create(Animal* self, const char* name);
void _Animal_Destroy(Animal* self);
Animal* _Animal_New(const char* name);
void _Animal_Delete(Animal* self);
String* _Animal_ToString(Animal* self);
void _Animal_Speak(void* self);
void _Animal_Walk(void* self, int steps);

/* ---- 方法实现 ---- */

static void _Animal_Speak(void* self)
{
    Animal* a = (Animal*)self;
    println(a->name, "says: ...");
}

static void _Animal_Walk(void* self, int steps)
{
    Animal* a = (Animal*)self;
    println(a->name, "walks", steps, "steps");
}

static String* _Animal_ToString(Animal* self)
{
    String* str = New(String, 32);
    Call(String, str, Append, "Animal(");
    Call(String, str, Append, self->name);
    Call(String, str, Append, ")");
    return str;
}

/* ---- Create/New/Destroy/Delete ---- */

inline void _Animal_Create(Animal* self, const char* name)
{
    static _Animal_VTable vt = {
        .create = _Animal_Create,
        .destroy = _Object_Destroy,
        .new = _Animal_New,
        .delete = _Animal_Delete,
        .to_string = _Animal_ToString,
        .speak = _Animal_Speak,
        .walk = _Animal_Walk,
    };
    Object_Create((Object*)self);
    self->vptr = &vt;
    self->name = name;
}

inline void _Animal_Destroy(Animal* self)
{
    _Object_Destroy((Object*)self);
}

inline Animal* _Animal_New(const char* name)
{
    Animal* self = (Animal*)malloc(sizeof(Animal));
    ERR_RET_V_NULL(self, NULL);
    _Animal_Create(self, name);
    return self;
}

inline void _Animal_Delete(Animal* self)
{
    _Animal_Destroy(self);
    free(self);
}

/* ================================================================
 * 步骤 2: 定义子类 Cat — 继承 Animal，重写 speak
 * ================================================================
 * VTABLE 继承 _Animal_VTable，新增可选的 cat 专用方法。
 * CLASS 嵌入 Animal（FROM(Animal)），自动拥有 name 字段。
 */

VTABLE{
    FROM(_Animal_VTable);         /* 继承 Animal 的虚表 */
    void (*purr)(void* self);     /* Cat 新增方法 */
} _Cat_VTable;

CLASS{
    FROM(Animal);                  /* 继承 Animal */
} Cat;

/* ---- 方法声明 ---- */
void _Cat_Create(Cat* self, const char* name);
void _Cat_Destroy(Cat* self);
Cat* _Cat_New(const char* name);
void _Cat_Delete(Cat* self);
String* _Cat_ToString(Cat* self);
void _Cat_Speak(void* self);
void _Cat_Walk(void* self, int steps);
void _Cat_Purr(void* self);

static void _Cat_Speak(void* self)
{
    Animal* a = (Animal*)self;
    println(a->name, "says: Meow!");
}

static void _Cat_Walk(void* self, int steps)
{
    Animal* a = (Animal*)self;
    println(a->name, "silently walks", steps, "steps");
}

static void _Cat_Purr(void* self)
{
    println(((Animal*)self)->name, "purrs: rrrrr...");
}

static String* _Cat_ToString(Cat* self)
{
    String* str = New(String, 0);
    Call(String, str, Append, "Cat(");
    Call(String, str, Append, ((Animal*)self)->name);
    Call(String, str, Append, ")");
    return str;
}

inline void _Cat_Create(Cat* self, const char* name)
{
    static _Cat_VTable vt = {
        .create = _Cat_Create,
        .destroy = _Object_Destroy,
        .new = _Cat_New,
        .delete = _Cat_Delete,
        .to_string = _Cat_ToString,
        .speak = _Cat_Speak,       /* 重写 */
        .walk = _Cat_Walk,        /* 重写 */
        .purr = _Cat_Purr,        /* 新增 */
    };
    Object_Create((Object*)self);
    self->vptr = &vt;
    ((Animal*)self)->name = name;
}

inline void _Cat_Destroy(Cat* self)
{
    _Object_Destroy((Object*)self);
}

inline Cat* _Cat_New(const char* name)
{
    Cat* self = (Cat*)malloc(sizeof(Cat));
    ERR_RET_V_NULL(self, NULL);
    _Cat_Create(self, name);
    return self;
}

inline void _Cat_Delete(Cat* self)
{
    _Cat_Destroy(self);
    free(self);
}

/* ================================================================
 * 步骤 3: 定义子类 Dog — 继承 Animal，只重写 speak
 * ================================================================
 */

VTABLE{
    FROM(_Animal_VTable);
} _Dog_VTable;                      /* 无新增方法，仅需继承 */

CLASS{
    FROM(Animal);
    bool is_wagging;
} Dog;

/* ---- 方法声明 ---- */
void _Dog_Create(Dog* self, const char* name);
void _Dog_Destroy(Dog* self);
Dog* _Dog_New(const char* name);
void _Dog_Delete(Dog* self);
String* _Dog_ToString(Dog* self);
void _Dog_Speak(void* self);
void _Dog_Walk(void* self, int steps);   // 沿用 Animal 的

static void _Dog_Speak(void* self)
{
    Animal* a = (Animal*)self;
    println(a->name, "says: Woof!");
}

static String* _Dog_ToString(Dog* self)
{
    String* str = New(String, 0);
    Call(String, str, Append, "Dog(");
    Call(String, str, Append, ((Animal*)self)->name);
    Call(String, str, Append, ")");
    return str;
}

inline void _Dog_Create(Dog* self, const char* name)
{
    static _Dog_VTable vt = {
        .create = _Dog_Create,
        .destroy = _Object_Destroy,
        .new = _Dog_New,
        .delete = _Dog_Delete,
        .to_string = _Dog_ToString,
        .speak = _Dog_Speak,       /* 重写 */
        .walk = _Animal_Walk,     /* 沿用基类实现 */
    };
    Object_Create((Object*)self);
    self->vptr = &vt;
    ((Animal*)self)->name = name;
    self->is_wagging = false;
}

inline void _Dog_Destroy(Dog* self)
{
    _Object_Destroy((Object*)self);
}

inline Dog* _Dog_New(const char* name)
{
    Dog* self = (Dog*)malloc(sizeof(Dog));
    ERR_RET_V_NULL(self, NULL);
    _Dog_Create(self, name);
    return self;
}

inline void _Dog_Delete(Dog* self)
{
    _Dog_Destroy(self);
    free(self);
}

/* ================================================================
 * 步骤 4: 演示多态
 *
 * VCall 和 FT 在函数调用上作用完全一致：
 *   VCall(T, self, func, ...)
 *   等价于
 *   FT(T, self)->func(self, ...)
 * ================================================================
 */

/* 多态函数：接受任意继承自 Animal 的对象，通过 vptr 派发 */
static void make_speak_twice(Animal* animal)
{
    println("--- make_speak_twice ---");

    /* VCall 通过 vptr 自动找到最派生类的 speak */
    VCall(Animal, animal, speak);
    /* FT 写法，作用完全相同 */
    FT(Animal, animal)->speak(animal);

    /* ToString 也是虚函数，通过 vptr 派发 */
    String* type_str = VCall(Animal, animal, to_string);
    println("type:", type_str);
    Call(String, type_str, Delete);
}

int main(void)
{
    /* ========== 栈分配对象 ========== */

    Animal a;
    Create(Animal, &a, "Generic");

    Cat    c;
    Create(Cat, &c, "Mimi");

    Dog    d;
    Create(Dog, &d, "Wangcai");

    /* ========== 虚函数调用：验证多态 ========== */

    println("=== 直接调用（静态类型决定）===");

    /* Animal → Animal::speak */
    FT(Animal, &a)->speak(&a);
    /* Cat → Cat::speak（重写） */
    FT(Animal, &c)->speak(&c);      /* 通过 Animal vptr 派发到 Cat::speak！ */
    /* Dog → Dog::speak（重写） */
    FT(Animal, &d)->speak(&d);      /* 通过 Animal vptr 派发到 Dog::speak！ */

    println("\n=== 多态参数传递 ===");
    make_speak_twice(&a);
    make_speak_twice(&c);
    make_speak_twice(&d);

    println("\n=== Cat 特有方法（需转型到 Cat vptr）===");
    /* Cat 特有的 purr 只能通过 Cat 类型的 vptr 调用 */
    VCall(Cat, &c, purr);
    /* 等价 FT 写法 */
    FT(Cat, &c)->purr(&c);

    println("\n=== Walk 验证：Cat 重写了 walk，Dog 沿用 Animal 的 walk ===");
    VCall(Animal, &c, walk, 3);     /* Cat::walk — "silently walks 3 steps" */
    VCall(Animal, &d, walk, 5);     /* Animal::walk（Dog 未重写）— "walks 5 steps" */

    VCall(Animal, &a, walk, 2);

    /* ========== 堆分配对象 ========== */

    println("\n=== 堆分配（New/Delete）===");

    Cat* heap_cat = New(Cat, "Kitty");
    VCall(Cat, heap_cat, speak);
    VCall(Cat, heap_cat, purr);
    Call(Cat, heap_cat, Delete);

    Dog* heap_dog = New(Dog, "Buddy");
    VCall(Dog, heap_dog, speak);
    Call(Dog, heap_dog, Delete);

    /* ========== 销毁栈对象 ========== */

    VCall(Animal, &a, destroy);
    VCall(Cat, &c, destroy);
    VCall(Dog, &d, destroy);

    /* 等价于直接 Call 函数（静态分发） */
    /* Call(Animal, &a, Destroy); */

    /* ========== 容器也是 Object ========== */

    println("\n=== 容器同样继承自 Object（vptr 机制一致）===");

    Vec(f32) v;
    Create(Vec(f32), &v);
    println("Vec is_empty:", VCall(Vec(f32), &v, is_empty));

    FT(Vec(f32), &v)->push_back(&v, 3.14f);
    println("VCall(back):", *VCall(Vec(f32), &v, back));
    /* 容器 vptr.destroy 等价于 VCall/VCall(…, destroy) */
    v.vptr->destroy(&v);

    return 0;
}