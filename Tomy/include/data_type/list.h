#pragma once
#include <string.h>

#include "data_type.h"
#include "ustring.h"
#include "class/object_class.h"
#include "vector.h"  /* _Iterator_VTable, ElemConstructor/Destructor/Copy */

/* ============ Node Type ============ */

typedef struct _ListNode { struct _ListNode* next; } _ListNode;

static inline void* _List_Data(_ListNode* node) { return (byte*)node + sizeof(_ListNode); }
static inline _ListNode* _List_NewNode(umax elem_size) { return (_ListNode*)malloc(sizeof(_ListNode) + elem_size); }

/* ============ _ListBase ============ */

VTABLE{ FROM(_Object_VTable); } _ListBase_VTable;

CLASS{
    FROM(Object);
    _ListNode head;   /* dummy head — head.next = first node or NULL */
    _ListNode* tail;  /* points to last real node, or &head when empty */
    umax size;
    umax elem_size;
    ElemConstructor* construct;
    ElemDestructor* destroy;
    ElemCopy* copy;
} _ListBase;

/* ============ _ListBase Implementation ============ */

inline void _ListBase_Create(_ListBase* self,
    umax elem_size,
    ElemConstructor* construct, ElemDestructor* destroy, ElemCopy* copy)
{
    ERR_RET_NULL(self);
    Object_Create((Object*)self);
    self->head.next = NULL;
    self->tail = &self->head;
    self->size = 0;
    self->elem_size = elem_size;
    self->construct = construct;
    self->destroy = destroy;
    self->copy = copy;
}

inline void _ListBase_Destroy(_ListBase* self)
{
    ERR_RET_NULL(self);
    _ListNode* curr = self->head.next;
    while (curr)
    {
        _ListNode* next = curr->next;
        if (self->destroy) self->destroy(_List_Data(curr));
        free(curr);
        curr = next;
    }
    self->head.next = NULL;
    self->tail = &self->head;
    self->size = 0;
}

inline _ListBase* _ListBase_New(umax elem_size,
    ElemConstructor* construct, ElemDestructor* destroy, ElemCopy* copy)
{
    _ListBase* self = (_ListBase*)malloc(sizeof(_ListBase));
    ERR_RET_V_NULL(self, NULL);
    _ListBase_Create(self, elem_size, construct, destroy, copy);
    return self;
}

inline void _ListBase_Delete(_ListBase* self)
{
    ERR_RET_NULL(self);
    _ListBase_Destroy(self);
    free(self);
}

/* ---- Mutators ---- */

inline void _ListBase_PushFront(_ListBase* self, const void* elem)
{
    ERR_RET_NULL(self);
    ERR_RET_NULL(elem);
    _ListNode* node = _List_NewNode(self->elem_size);
    ERR_RET_NULL(node);

    node->next = self->head.next;
    self->head.next = node;

    void* data = _List_Data(node);
    if (self->copy) self->copy(data, elem);
    else memcpy(data, elem, self->elem_size);

    if (self->tail == &self->head) self->tail = node;
    self->size++;
}

inline void _ListBase_PopFront(_ListBase* self)
{
    ERR_RET_NULL(self);
    if (self->size == 0) return;

    _ListNode* node = self->head.next;
    self->head.next = node->next;

    if (self->tail == node) self->tail = &self->head;

    if (self->destroy) self->destroy(_List_Data(node));
    free(node);
    self->size--;
}

inline void _ListBase_PushBack(_ListBase* self, const void* elem)
{
    ERR_RET_NULL(self);
    ERR_RET_NULL(elem);
    _ListNode* node = _List_NewNode(self->elem_size);
    ERR_RET_NULL(node);

    node->next = NULL;
    self->tail->next = node;
    self->tail = node;

    void* data = _List_Data(node);
    if (self->copy) self->copy(data, elem);
    else memcpy(data, elem, self->elem_size);

    self->size++;
}

/* ---- Accessors ---- */

inline void* _ListBase_Front(const _ListBase* self)
{
    ERR_RET_V_NULL(self, NULL);
    if (self->size == 0) return NULL;
    return _List_Data(self->head.next);
}

inline void* _ListBase_Back(const _ListBase* self)
{
    ERR_RET_V_NULL(self, NULL);
    if (self->size == 0) return NULL;
    return _List_Data(self->tail);
}

inline void _ListBase_InsertAfter(_ListBase* self, _ListNode* pos, const void* elem)
{
    if (!pos) return;
    _ListNode* node = _List_NewNode(self->elem_size);
    if (!node) return;

    node->next = pos->next;
    pos->next = node;

    void* data = _List_Data(node);
    if (self->copy) self->copy(data, elem);
    else memcpy(data, elem, self->elem_size);

    if (pos == self->tail) self->tail = node;
    self->size++;
}

inline void _ListBase_EraseAfter(_ListBase* self, _ListNode* pos)
{
    ERR_RET_NULL(self);
    if (!pos || !pos->next) return;

    _ListNode* target = pos->next;
    pos->next = target->next;

    if (self->tail == target) self->tail = pos;

    if (self->destroy) self->destroy(_List_Data(target));
    free(target);
    self->size--;
}

inline void _ListBase_Clear(_ListBase* self)
{
    ERR_RET_NULL(self);
    _ListNode* curr = self->head.next;
    while (curr)
    {
        _ListNode* next = curr->next;
        if (self->destroy) self->destroy(_List_Data(curr));
        free(curr);
        curr = next;
    }
    self->head.next = NULL;
    self->tail = &self->head;
    self->size = 0;
}

inline bool _ListBase_IsEmpty(const _ListBase* self)
{
    ERR_RET_V_NULL(self, false);
    return self->size == 0;
}

inline umax _ListBase_Size(const _ListBase* self)
{
    ERR_RET_V_NULL(self, 0);
    return self->size;
}

inline void _ListBase_Reverse(_ListBase* self)
{
    ERR_RET_NULL(self);
    _ListNode* prev = NULL;
    _ListNode* curr = self->head.next;
    self->tail = curr ? curr : &self->head;
    while (curr)
    {
        _ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    self->head.next = prev;
}

/* ============ X-Macro: Typed List Generation ============ */

#define _LIST_IMPL_EX1(T, CONSTRUCT, DESTROY, COPY)                                            \
                                                                                                \
    /* ---- Iterator VTABLE + CLASS ---- */                                                     \
    VTABLE{                                                                                     \
        FROM(_Iterator_VTable);                                                                 \
        T (*get)(const void* self);                                                             \
    } _List_##T##_Iterator##_VTable;                                                            \
    CLASS{                                                                                      \
        FROM(Iterator);                                                                         \
        _ListNode* node;                                                                        \
    } List_##T##_Iterator;                                                                      \
                                                                                                \
    /* ---- List VTABLE ---- */                                                                 \
    VTABLE{                                                                                     \
        FROM(_ListBase_VTable);                                                                 \
        List_##T##_Iterator (*begin)(const void* self);                                         \
        List_##T##_Iterator (*end)(const void* self);                                           \
        List_##T##_Iterator (*before_begin)(const void* self);                                  \
        void (*push_front)(void* self, T val);                                                  \
        void (*pop_front)(void* self);                                                          \
        void (*push_back)(void* self, T val);                                                   \
        T* (*front)(const void* self);                                                          \
        T* (*back)(const void* self);                                                           \
        void (*insert_after)(void* self, List_##T##_Iterator pos, T val);                      \
        void (*erase_after)(void* self, List_##T##_Iterator pos);                              \
        void (*clear)(void* self);                                                              \
        bool (*is_empty)(const void* self);                                                     \
        umax (*size)(const void* self);                                                         \
        void (*reverse)(void* self);                                                            \
    } _List_##T##_VTable;                                                                       \
                                                                                                \
    CLASS{ FROM(_ListBase); } List_##T;                                                         \
                                                                                                \
    /* ===== Iterator declarations ===== */                                                     \
    void _List_##T##_Iterator##_Create(List_##T##_Iterator* self);                              \
    void _List_##T##_Iterator##_Destroy(List_##T##_Iterator* self);                             \
    List_##T##_Iterator* _List_##T##_Iterator##_New();                                          \
    void _List_##T##_Iterator##_Delete(List_##T##_Iterator* self);                              \
    String* _List_##T##_Iterator##_ToString(List_##T##_Iterator* self);                         \
    void* _List_##T##_Iterator##_Raw(const List_##T##_Iterator* self);                          \
    T _List_##T##_Iterator##_Get(const List_##T##_Iterator* self);                              \
    void _List_##T##_Iterator##_Next(List_##T##_Iterator* self);                                \
    bool _List_##T##_Iterator##_Equals(                                                         \
        const List_##T##_Iterator* self,                                                        \
        const List_##T##_Iterator* other);                                                      \
                                                                                                \
    /* ===== List declarations ===== */                                                         \
    void _List_##T##_Create(List_##T* self);                                                    \
    void _List_##T##_Destroy(List_##T* self);                                                   \
    List_##T* _List_##T##_New();                                                                \
    void _List_##T##_Delete(List_##T* self);                                                    \
    String* _List_##T##_ToString(List_##T* self);                                               \
    void _List_##T##_PushFront(List_##T* self, T val);                                          \
    void _List_##T##_PopFront(List_##T* self);                                                  \
    void _List_##T##_PushBack(List_##T* self, T val);                                           \
    T* _List_##T##_Front(const List_##T* self);                                                 \
    T* _List_##T##_Back(const List_##T* self);                                                  \
    void _List_##T##_InsertAfter(List_##T* self, List_##T##_Iterator pos, T val);              \
    void _List_##T##_EraseAfter(List_##T* self, List_##T##_Iterator pos);                      \
    void _List_##T##_Clear(List_##T* self);                                                     \
    bool _List_##T##_IsEmpty(const List_##T* self);                                             \
    umax _List_##T##_Size(const List_##T* self);                                                \
    void _List_##T##_Reverse(List_##T* self);                                                   \
    List_##T##_Iterator _List_##T##_Begin(const List_##T* self);                               \
    List_##T##_Iterator _List_##T##_End(const List_##T* self);                                 \
    List_##T##_Iterator _List_##T##_BeforeBegin(const List_##T* self);                         \
                                                                                                \
    /* ===== Iterator Inline Definitions ===== */                                               \
    inline void _List_##T##_Iterator##_Create(List_##T##_Iterator* self)                        \
    {                                                                                           \
        static _List_##T##_Iterator##_VTable vt = {                                            \
            _List_##T##_Iterator##_Create,                                                     \
            _List_##T##_Iterator##_Destroy,                                                    \
            _List_##T##_Iterator##_New,                                                        \
            _List_##T##_Iterator##_Delete,                                                     \
            _List_##T##_Iterator##_ToString,                                                   \
            .raw    = _List_##T##_Iterator##_Raw,                                              \
            .next   = _List_##T##_Iterator##_Next,                                             \
            .equals = _List_##T##_Iterator##_Equals,                                           \
            .get    = _List_##T##_Iterator##_Get,                                              \
        };                                                                                      \
        Object_Create((Object*)self);                                                           \
        ((Object*)self)->vptr = (void*)&vt;                                                     \
        self->node = NULL;                                                                      \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_Iterator##_Destroy(List_##T##_Iterator* self)                       \
    {                                                                                           \
        _Object_Destroy((Object*)self);                                                         \
    }                                                                                           \
                                                                                                \
    inline List_##T##_Iterator* _List_##T##_Iterator##_New()                                    \
    {                                                                                           \
        List_##T##_Iterator* self =                                                             \
            (List_##T##_Iterator*)malloc(sizeof(List_##T##_Iterator));                          \
        ERR_RET_V_NULL(self, NULL);                                                             \
        _List_##T##_Iterator##_Create(self);                                                    \
        return self;                                                                            \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_Iterator##_Delete(List_##T##_Iterator* self)                        \
    {                                                                                           \
        _List_##T##_Iterator##_Destroy(self);                                                   \
        free(self);                                                                             \
    }                                                                                           \
                                                                                                \
    inline String* _List_##T##_Iterator##_ToString(List_##T##_Iterator* self)                   \
    {                                                                                           \
        String* str = New(String, STRING_CAPACITY);                                             \
        Call(String, str, Append, "ListIterator");                                              \
        return str;                                                                             \
    }                                                                                           \
                                                                                                \
    inline void* _List_##T##_Iterator##_Raw(const List_##T##_Iterator* self)                    \
    {                                                                                           \
        if (!self->node) return NULL;                                                           \
        return _List_Data(self->node);                                                          \
    }                                                                                           \
                                                                                                \
    inline T _List_##T##_Iterator##_Get(const List_##T##_Iterator* self)                        \
    {                                                                                           \
        if (!self->node) return (T){0};                                                        \
        return *(T*)_List_Data(self->node);                                                     \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_Iterator##_Next(List_##T##_Iterator* self)                          \
    {                                                                                           \
        if (self->node) self->node = self->node->next;                                          \
    }                                                                                           \
                                                                                                \
    inline bool _List_##T##_Iterator##_Equals(                                                  \
        const List_##T##_Iterator* self,                                                        \
        const List_##T##_Iterator* other)                                                       \
    {                                                                                           \
        return self->node == other->node;                                                       \
    }                                                                                           \
                                                                                                \
    /* ===== List Inline Definitions ===== */                                                   \
    inline void _List_##T##_Create(List_##T* self)                                              \
    {                                                                                           \
        static _List_##T##_VTable vt = {                                                       \
            _List_##T##_Create,                                                                 \
            _List_##T##_Destroy,                                                                \
            _List_##T##_New,                                                                    \
            _List_##T##_Delete,                                                                 \
            _List_##T##_ToString,                                                               \
            .begin        = _List_##T##_Begin,                                                  \
            .end          = _List_##T##_End,                                                    \
            .before_begin = _List_##T##_BeforeBegin,                                            \
            .push_front   = _List_##T##_PushFront,                                              \
            .pop_front    = _List_##T##_PopFront,                                               \
            .push_back    = _List_##T##_PushBack,                                               \
            .front        = _List_##T##_Front,                                                  \
            .back         = _List_##T##_Back,                                                   \
            .insert_after = _List_##T##_InsertAfter,                                            \
            .erase_after  = _List_##T##_EraseAfter,                                             \
            .clear        = _List_##T##_Clear,                                                  \
            .is_empty     = _List_##T##_IsEmpty,                                                \
            .size         = _List_##T##_Size,                                                   \
            .reverse      = _List_##T##_Reverse,                                                \
        };                                                                                      \
        _ListBase_Create((_ListBase*)self, sizeof(T), CONSTRUCT, DESTROY, COPY);                \
        self->vptr = (void*)&vt;                                                                \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_Destroy(List_##T* self)                                             \
    {                                                                                           \
        _ListBase_Destroy((_ListBase*)self);                                                    \
    }                                                                                           \
                                                                                                \
    inline List_##T* _List_##T##_New()                                                          \
    {                                                                                           \
        List_##T* self = (List_##T*)malloc(sizeof(List_##T));                                   \
        ERR_RET_V_NULL(self, NULL);                                                             \
        _List_##T##_Create(self);                                                               \
        return self;                                                                            \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_Delete(List_##T* self)                                              \
    {                                                                                           \
        _List_##T##_Destroy(self);                                                              \
        free(self);                                                                             \
    }                                                                                           \
                                                                                                \
    inline String* _List_##T##_ToString(List_##T* self)                                         \
    {                                                                                           \
        String* str = New(String, STRING_CAPACITY);                                             \
        Call(String, str, Append, "List<");                                                     \
        Call(String, str, Append, #T);                                                          \
        Call(String, str, Append, ">");                                                         \
        return str;                                                                             \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_PushFront(List_##T* self, T val)                                    \
    {                                                                                           \
        _ListBase_PushFront((_ListBase*)self, &val);                                            \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_PopFront(List_##T* self)                                            \
    {                                                                                           \
        _ListBase_PopFront((_ListBase*)self);                                                   \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_PushBack(List_##T* self, T val)                                     \
    {                                                                                           \
        _ListBase_PushBack((_ListBase*)self, &val);                                             \
    }                                                                                           \
                                                                                                \
    inline T* _List_##T##_Front(const List_##T* self)                                           \
    {                                                                                           \
        return (T*)_ListBase_Front((const _ListBase*)self);                                     \
    }                                                                                           \
                                                                                                \
    inline T* _List_##T##_Back(const List_##T* self)                                            \
    {                                                                                           \
        return (T*)_ListBase_Back((const _ListBase*)self);                                      \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_InsertAfter(List_##T* self,                                         \
        List_##T##_Iterator pos, T val)                                                         \
    {                                                                                           \
        _ListBase_InsertAfter((_ListBase*)self, pos.node, &val);                                \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_EraseAfter(List_##T* self,                                          \
        List_##T##_Iterator pos)                                                                \
    {                                                                                           \
        _ListBase_EraseAfter((_ListBase*)self, pos.node);                                       \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_Clear(List_##T* self)                                               \
    {                                                                                           \
        _ListBase_Clear((_ListBase*)self);                                                      \
    }                                                                                           \
                                                                                                \
    inline bool _List_##T##_IsEmpty(const List_##T* self)                                       \
    {                                                                                           \
        return _ListBase_IsEmpty((const _ListBase*)self);                                       \
    }                                                                                           \
                                                                                                \
    inline umax _List_##T##_Size(const List_##T* self)                                          \
    {                                                                                           \
        return _ListBase_Size((const _ListBase*)self);                                          \
    }                                                                                           \
                                                                                                \
    inline void _List_##T##_Reverse(List_##T* self)                                             \
    {                                                                                           \
        _ListBase_Reverse((_ListBase*)self);                                                    \
    }                                                                                           \
                                                                                                \
    inline List_##T##_Iterator _List_##T##_Begin(const List_##T* self)                          \
    {                                                                                           \
        List_##T##_Iterator it;                                                                 \
        _List_##T##_Iterator##_Create(&it);                                                     \
        it.node = ((_ListBase*)self)->head.next;                                                \
        return it;                                                                              \
    }                                                                                           \
                                                                                                \
    inline List_##T##_Iterator _List_##T##_End(const List_##T* self)                            \
    {                                                                                           \
        List_##T##_Iterator it;                                                                 \
        _List_##T##_Iterator##_Create(&it);                                                     \
        it.node = NULL;                                                                         \
        return it;                                                                              \
    }                                                                                           \
                                                                                                \
    inline List_##T##_Iterator _List_##T##_BeforeBegin(const List_##T* self)                    \
    {                                                                                           \
        List_##T##_Iterator it;                                                                 \
        _List_##T##_Iterator##_Create(&it);                                                     \
        it.node = &((_ListBase*)self)->head;                                                    \
        return it;                                                                              \
    }

#define _LIST_IMPL_EX2(T, CONSTRUCT, DESTROY, COPY)  _LIST_IMPL_EX1(T, CONSTRUCT, DESTROY, COPY)
#define _LIST_IMPL_EX3(T, CONSTRUCT, DESTROY, COPY)  _LIST_IMPL_EX2(T, CONSTRUCT, DESTROY, COPY)
#define LIST_IMPL_EX(T, CONSTRUCT, DESTROY, COPY)    _LIST_IMPL_EX3(T, CONSTRUCT, DESTROY, COPY)

/* For POD types: NULL callbacks → memcpy-based */
#define LIST_IMPL(T)  LIST_IMPL_EX(T, NULL, NULL, NULL)

/* ============ Type Shortcut Macros ============ */

#define _List(T)     List_##T
#define List(T)      _List(T)
#define _ListIter(T) List_##T##_Iterator
#define ListIter(T)  _ListIter(T)

/* ============ Pre-instantiated Types ============ */

LIST_IMPL(i8);
LIST_IMPL(i16);
LIST_IMPL(i32);
LIST_IMPL(i64);
LIST_IMPL(imax);
LIST_IMPL(u8);
LIST_IMPL(u16);
LIST_IMPL(u32);
LIST_IMPL(u64);
LIST_IMPL(umax);
LIST_IMPL(f32);
LIST_IMPL(f64);
LIST_IMPL(Object);

LIST_IMPL_EX(String, _String_Create, _String_Destroy, _String_Copy);
