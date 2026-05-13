#pragma once
#include <string.h>

#include "data_type.h"
#include "ustring.h"
#include "class/object_class.h"
#include "vector.h"  /* _Iterator_VTable, ElemConstructor/Destructor/Copy */

/* ============ Node Type ============ */

typedef struct _DListNode
{
    struct _DListNode* prev;
    struct _DListNode* next;
} _DListNode;

static inline void* _DList_Data(_DListNode* node)
{
    return (byte*)node + sizeof(_DListNode);
}
static inline _DListNode* _DList_NewNode(umax elem_size)
{
    return (_DListNode*)malloc(sizeof(_DListNode) + elem_size);
}

/* Circular sentinel helpers */
static inline void _DList_InitSentinel(_DListNode* sentinel)
{
    sentinel->prev = sentinel;
    sentinel->next = sentinel;
}

static inline bool _DList_IsSingleton(const _DListNode* sentinel)
{
    return sentinel->next != sentinel && sentinel->next->next == sentinel;
}

static inline void _DList_InsertBefore(_DListNode* pos, _DListNode* node)
{
    node->prev = pos->prev;
    node->next = pos;
    pos->prev->next = node;
    pos->prev = node;
}

static inline void _DList_Remove(_DListNode* node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

/* ============ _DoublyListBase ============ */

VTABLE{ FROM(_Object_VTable); } _DoublyListBase_VTable;

CLASS{
    FROM(Object);
    _DListNode head;  /* circular sentinel — head.next = first, head.prev = last */
    umax size;
    umax elem_size;
    ElemConstructor* construct;
    ElemDestructor* destroy;
    ElemCopy* copy;
    CmpFunc cmp;
} _DoublyListBase;

/* ============ _DoublyListBase Implementation ============ */

inline void _DoublyListBase_Create(_DoublyListBase* self,
    umax elem_size,
    ElemConstructor* construct, ElemDestructor* destroy, ElemCopy* copy, CmpFunc cmp)
{
    ERR_RET_NULL(self);
    Object_Create((Object*)self);
    _DList_InitSentinel(&self->head);
    self->size = 0;
    self->elem_size = elem_size;
    self->construct = construct;
    self->destroy = destroy;
    self->copy = copy;
    self->cmp = cmp;
}

inline void _DoublyListBase_Destroy(_DoublyListBase* self)
{
    ERR_RET_NULL(self);
    _DListNode* curr = self->head.next;
    while (curr != &self->head)
    {
        _DListNode* next = curr->next;
        if (self->destroy) self->destroy(_DList_Data(curr));
        free(curr);
        curr = next;
    }
    _DList_InitSentinel(&self->head);
    self->size = 0;
}

inline _DoublyListBase* _DoublyListBase_New(umax elem_size,
    ElemConstructor* construct, ElemDestructor* destroy, ElemCopy* copy, CmpFunc cmp)
{
    _DoublyListBase* self = (_DoublyListBase*)malloc(sizeof(_DoublyListBase));
    ERR_RET_V_NULL(self, NULL);
    _DoublyListBase_Create(self, elem_size, construct, destroy, copy, cmp);
    return self;
}

inline void _DoublyListBase_Delete(_DoublyListBase* self)
{
    ERR_RET_NULL(self);
    _DoublyListBase_Destroy(self);
    free(self);
}

/* ---- Mutators ---- */

inline void _DoublyListBase_PushFront(_DoublyListBase* self, const void* elem)
{
    ERR_RET_NULL(self);
    ERR_RET_NULL(elem);
    _DListNode* node = _DList_NewNode(self->elem_size);
    ERR_RET_NULL(node);

    _DList_InsertBefore(self->head.next, node);
    void* data = _DList_Data(node);
    if (self->copy) self->copy(data, elem);
    else memcpy(data, elem, self->elem_size);
    self->size++;
}

inline void _DoublyListBase_PopFront(_DoublyListBase* self)
{
    ERR_RET_NULL(self);
    if (self->size == 0) return;

    _DListNode* node = self->head.next;
    _DList_Remove(node);
    if (self->destroy) self->destroy(_DList_Data(node));
    free(node);
    self->size--;
}

inline void _DoublyListBase_PushBack(_DoublyListBase* self, const void* elem)
{
    ERR_RET_NULL(self);
    ERR_RET_NULL(elem);
    _DListNode* node = _DList_NewNode(self->elem_size);
    ERR_RET_NULL(node);

    _DList_InsertBefore(&self->head, node);
    void* data = _DList_Data(node);
    if (self->copy) self->copy(data, elem);
    else memcpy(data, elem, self->elem_size);
    self->size++;
}

inline void _DoublyListBase_PopBack(_DoublyListBase* self)
{
    ERR_RET_NULL(self);
    if (self->size == 0) return;

    _DListNode* node = self->head.prev;
    _DList_Remove(node);
    if (self->destroy) self->destroy(_DList_Data(node));
    free(node);
    self->size--;
}

/* ---- Accessors ---- */

inline void* _DoublyListBase_Front(const _DoublyListBase* self)
{
    ERR_RET_V_NULL(self, NULL);
    if (self->size == 0) return NULL;
    return _DList_Data(self->head.next);
}

inline void* _DoublyListBase_Back(const _DoublyListBase* self)
{
    ERR_RET_V_NULL(self, NULL);
    if (self->size == 0) return NULL;
    return _DList_Data(self->head.prev);
}

inline void _DoublyListBase_InsertBefore(_DoublyListBase* self, _DListNode* pos, const void* elem)
{
    ERR_RET_NULL(self);
    ERR_RET_NULL(elem);
    if (!pos) return;

    _DListNode* node = _DList_NewNode(self->elem_size);
    if (!node) return;

    _DList_InsertBefore(pos, node);
    void* data = _DList_Data(node);
    if (self->copy) self->copy(data, elem);
    else memcpy(data, elem, self->elem_size);
    self->size++;
}

inline void _DoublyListBase_Erase(_DoublyListBase* self, _DListNode* pos)
{
    ERR_RET_NULL(self);
    if (!pos || pos == &self->head) return;

    _DList_Remove(pos);
    if (self->destroy) self->destroy(_DList_Data(pos));
    free(pos);
    self->size--;
}

inline void _DoublyListBase_Clear(_DoublyListBase* self)
{
    ERR_RET_NULL(self);
    _DListNode* curr = self->head.next;
    while (curr != &self->head)
    {
        _DListNode* next = curr->next;
        if (self->destroy) self->destroy(_DList_Data(curr));
        free(curr);
        curr = next;
    }
    _DList_InitSentinel(&self->head);
    self->size = 0;
}

inline bool _DoublyListBase_IsEmpty(const _DoublyListBase* self)
{
    ERR_RET_V_NULL(self, false);
    return self->size == 0;
}

inline umax _DoublyListBase_Size(const _DoublyListBase* self)
{
    ERR_RET_V_NULL(self, 0);
    return self->size;
}

inline void _DoublyListBase_Reverse(_DoublyListBase* self)
{
    ERR_RET_NULL(self);
    if (self->size <= 1) return;

    _DListNode* curr = self->head.next;
    while (curr != &self->head)
    {
        _DListNode* next = curr->next;
        curr->next = curr->prev;
        curr->prev = next;
        curr = next;
    }
    /* Swap head.next and head.prev */
    _DListNode* tmp = self->head.next;
    self->head.next = self->head.prev;
    self->head.prev = tmp;
}

/* ============ X-Macro: Typed DoublyList Generation ============ */

#define _DLIST_IMPL_EX1(T, CONSTRUCT, DESTROY, COPY, CMP)                                               \
                                                                                                   \
    /* ---- Iterator VTABLE + CLASS ---- */                                                        \
    VTABLE{                                                                                        \
        FROM(_Iterator_VTable);                                                                    \
        T (*get)(const void* self);                                                                \
    } _DoublyList_##T##_Iterator##_VTable;                                                         \
    CLASS{                                                                                         \
        FROM(Iterator);                                                                            \
        _DListNode* node;                                                                          \
        const _DoublyListBase* list;  /* for sentinel detection */                                 \
    } DoublyList_##T##_Iterator;                                                                   \
                                                                                                   \
    /* ---- DoublyList VTABLE ---- */                                                              \
    VTABLE{                                                                                        \
        FROM(_DoublyListBase_VTable);                                                              \
        DoublyList_##T##_Iterator (*begin)(const void* self);                                      \
        DoublyList_##T##_Iterator (*end)(const void* self);                                        \
        void (*push_front)(void* self, T val);                                                     \
        void (*pop_front)(void* self);                                                             \
        void (*push_back)(void* self, T val);                                                      \
        void (*pop_back)(void* self);                                                              \
        T* (*front)(const void* self);                                                             \
        T* (*back)(const void* self);                                                              \
        void (*insert)(void* self, DoublyList_##T##_Iterator pos, T val);                          \
        void (*erase)(void* self, DoublyList_##T##_Iterator pos);                                  \
        void (*clear)(void* self);                                                                 \
        bool (*is_empty)(const void* self);                                                        \
        umax (*size)(const void* self);                                                            \
        void (*reverse)(void* self);                                                               \
    } _DoublyList_##T##_VTable;                                                                    \
                                                                                                   \
    CLASS{ FROM(_DoublyListBase); } DoublyList_##T;                                                \
                                                                                                   \
    /* ===== Iterator declarations ===== */                                                        \
    void _DoublyList_##T##_Iterator##_Create(DoublyList_##T##_Iterator* self);                     \
    void _DoublyList_##T##_Iterator##_Destroy(DoublyList_##T##_Iterator* self);                    \
    DoublyList_##T##_Iterator* _DoublyList_##T##_Iterator##_New();                                 \
    void _DoublyList_##T##_Iterator##_Delete(DoublyList_##T##_Iterator* self);                     \
    String* _DoublyList_##T##_Iterator##_ToString(DoublyList_##T##_Iterator* self);                \
    void* _DoublyList_##T##_Iterator##_Raw(const DoublyList_##T##_Iterator* self);                 \
    T _DoublyList_##T##_Iterator##_Get(const DoublyList_##T##_Iterator* self);                     \
    void _DoublyList_##T##_Iterator##_Next(DoublyList_##T##_Iterator* self);                       \
    bool _DoublyList_##T##_Iterator##_Equals(                                                      \
        const DoublyList_##T##_Iterator* self,                                                     \
        const DoublyList_##T##_Iterator* other);                                                   \
                                                                                                   \
    /* ===== DoublyList declarations ===== */                                                      \
    void _DoublyList_##T##_Create(DoublyList_##T* self);                                           \
    void _DoublyList_##T##_Destroy(DoublyList_##T* self);                                          \
    DoublyList_##T* _DoublyList_##T##_New();                                                       \
    void _DoublyList_##T##_Delete(DoublyList_##T* self);                                           \
    String* _DoublyList_##T##_ToString(DoublyList_##T* self);                                      \
    void _DoublyList_##T##_PushFront(DoublyList_##T* self, T val);                                 \
    void _DoublyList_##T##_PopFront(DoublyList_##T* self);                                         \
    void _DoublyList_##T##_PushBack(DoublyList_##T* self, T val);                                  \
    void _DoublyList_##T##_PopBack(DoublyList_##T* self);                                          \
    T* _DoublyList_##T##_Front(const DoublyList_##T* self);                                        \
    T* _DoublyList_##T##_Back(const DoublyList_##T* self);                                         \
    void _DoublyList_##T##_Insert(DoublyList_##T* self, DoublyList_##T##_Iterator pos, T val);     \
    void _DoublyList_##T##_Erase(DoublyList_##T* self, DoublyList_##T##_Iterator pos);             \
    void _DoublyList_##T##_Clear(DoublyList_##T* self);                                            \
    bool _DoublyList_##T##_IsEmpty(const DoublyList_##T* self);                                    \
    umax _DoublyList_##T##_Size(const DoublyList_##T* self);                                       \
    void _DoublyList_##T##_Reverse(DoublyList_##T* self);                                          \
    DoublyList_##T##_Iterator _DoublyList_##T##_Begin(const DoublyList_##T* self);                 \
    DoublyList_##T##_Iterator _DoublyList_##T##_End(const DoublyList_##T* self);                   \
                                                                                                   \
    /* ===== Iterator Inline Definitions ===== */                                                  \
    inline void _DoublyList_##T##_Iterator##_Create(DoublyList_##T##_Iterator* self)               \
    {                                                                                              \
        static _DoublyList_##T##_Iterator##_VTable vt = {                                          \
            _DoublyList_##T##_Iterator##_Create,                                                   \
            _DoublyList_##T##_Iterator##_Destroy,                                                  \
            _DoublyList_##T##_Iterator##_New,                                                      \
            _DoublyList_##T##_Iterator##_Delete,                                                   \
            _DoublyList_##T##_Iterator##_ToString,                                                 \
            .raw    = _DoublyList_##T##_Iterator##_Raw,                                            \
            .next   = _DoublyList_##T##_Iterator##_Next,                                           \
            .equals = _DoublyList_##T##_Iterator##_Equals,                                         \
            .get    = _DoublyList_##T##_Iterator##_Get,                                            \
        };                                                                                         \
        Object_Create((Object*)self);                                                              \
        ((Object*)self)->vptr = (void*)&vt;                                                        \
        self->node = NULL;                                                                         \
        self->list = NULL;                                                                         \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Iterator##_Destroy(DoublyList_##T##_Iterator* self)              \
    {                                                                                              \
        _Object_Destroy((Object*)self);                                                            \
    }                                                                                              \
                                                                                                   \
    inline DoublyList_##T##_Iterator* _DoublyList_##T##_Iterator##_New()                           \
    {                                                                                              \
        DoublyList_##T##_Iterator* self =                                                          \
            (DoublyList_##T##_Iterator*)malloc(sizeof(DoublyList_##T##_Iterator));                 \
        ERR_RET_V_NULL(self, NULL);                                                                \
        _DoublyList_##T##_Iterator##_Create(self);                                                 \
        return self;                                                                               \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Iterator##_Delete(DoublyList_##T##_Iterator* self)               \
    {                                                                                              \
        _DoublyList_##T##_Iterator##_Destroy(self);                                                \
        free(self);                                                                                \
    }                                                                                              \
                                                                                                   \
    inline String* _DoublyList_##T##_Iterator##_ToString(DoublyList_##T##_Iterator* self)          \
    {                                                                                              \
        String* str = New(String, STRING_CAPACITY);                                                \
        Call(String, str, Append, "DoublyListIterator");                                           \
        return str;                                                                                \
    }                                                                                              \
                                                                                                   \
    inline void* _DoublyList_##T##_Iterator##_Raw(const DoublyList_##T##_Iterator* self)           \
    {                                                                                              \
        ERR_RET_V_NULL_MSG(self, NULL, "Iterator is null");                                        \
        ERR_RET_V_NULL_MSG(self->node, NULL, "Raw() on null node");                                \
        ERR_RET_V_COND_MSG(self->node == &self->list->head, NULL,"Raw() on end() iterator");       \
        return _DList_Data(self->node);                                                            \
    }                                                                                              \
                                                                                                   \
    inline T _DoublyList_##T##_Iterator##_Get(const DoublyList_##T##_Iterator* self)               \
    {                                                                                              \
        ERR_RET_V_NULL_MSG(self, (T){0}, "Iterator is null");                                      \
        ERR_RET_V_NULL_MSG(self->node, (T){0}, "Get() on null node");                              \
        ERR_RET_V_COND_MSG(self->node == &self->list->head, (T){0}, "Get() on end() iterator");    \
        return *(T*)_DList_Data(self->node);                                                       \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Iterator##_Next(DoublyList_##T##_Iterator* self)                 \
    {                                                                                              \
        ERR_RET_NULL_MSG(self, "Iterator is null");                                                \
        ERR_RET_NULL_MSG(self->node, "Next() on null node");                                       \
        ERR_RET_V_COND_MSG(self->node == &self->list->head, , "Next() on end() iterator");         \
        self->node = self->node->next;                                                             \
    }                                                                                              \
                                                                                                   \
    inline bool _DoublyList_##T##_Iterator##_Equals(                                               \
        const DoublyList_##T##_Iterator* self,                                                     \
        const DoublyList_##T##_Iterator* other)                                                    \
    {                                                                                              \
        return self->node == other->node;                                                          \
    }                                                                                              \
                                                                                                   \
    /* ===== DoublyList Inline Definitions ===== */                                                \
    inline void _DoublyList_##T##_Create(DoublyList_##T* self)                                     \
    {                                                                                              \
        static _DoublyList_##T##_VTable vt = {                                                     \
            _DoublyList_##T##_Create,                                                              \
            _DoublyList_##T##_Destroy,                                                             \
            _DoublyList_##T##_New,                                                                 \
            _DoublyList_##T##_Delete,                                                              \
            _DoublyList_##T##_ToString,                                                            \
            .begin      = _DoublyList_##T##_Begin,                                                 \
            .end        = _DoublyList_##T##_End,                                                   \
            .push_front = _DoublyList_##T##_PushFront,                                             \
            .pop_front  = _DoublyList_##T##_PopFront,                                              \
            .push_back  = _DoublyList_##T##_PushBack,                                              \
            .pop_back   = _DoublyList_##T##_PopBack,                                               \
            .front      = _DoublyList_##T##_Front,                                                 \
            .back       = _DoublyList_##T##_Back,                                                  \
            .insert     = _DoublyList_##T##_Insert,                                                \
            .erase      = _DoublyList_##T##_Erase,                                                 \
            .clear      = _DoublyList_##T##_Clear,                                                 \
            .is_empty   = _DoublyList_##T##_IsEmpty,                                               \
            .size       = _DoublyList_##T##_Size,                                                  \
            .reverse    = _DoublyList_##T##_Reverse,                                               \
        };                                                                                         \
        _DoublyListBase_Create((_DoublyListBase*)self, sizeof(T), CONSTRUCT, DESTROY, COPY, CMP);       \
        self->vptr = (void*)&vt;                                                                   \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Destroy(DoublyList_##T* self)                                    \
    {                                                                                              \
        _DoublyListBase_Destroy((_DoublyListBase*)self);                                           \
    }                                                                                              \
                                                                                                   \
    inline DoublyList_##T* _DoublyList_##T##_New()                                                 \
    {                                                                                              \
        DoublyList_##T* self = (DoublyList_##T*)malloc(sizeof(DoublyList_##T));                    \
        ERR_RET_V_NULL(self, NULL);                                                                \
        _DoublyList_##T##_Create(self);                                                            \
        return self;                                                                               \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Delete(DoublyList_##T* self)                                     \
    {                                                                                              \
        _DoublyList_##T##_Destroy(self);                                                           \
        free(self);                                                                                \
    }                                                                                              \
                                                                                                   \
    inline String* _DoublyList_##T##_ToString(DoublyList_##T* self)                                \
    {                                                                                              \
        String* str = New(String, STRING_CAPACITY);                                                \
        Call(String, str, Append, "DoublyList<");                                                  \
        Call(String, str, Append, #T);                                                             \
        Call(String, str, Append, ">");                                                            \
        return str;                                                                                \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_PushFront(DoublyList_##T* self, T val)                           \
    {                                                                                              \
        _DoublyListBase_PushFront((_DoublyListBase*)self, &val);                                   \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_PopFront(DoublyList_##T* self)                                   \
    {                                                                                              \
        _DoublyListBase_PopFront((_DoublyListBase*)self);                                          \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_PushBack(DoublyList_##T* self, T val)                            \
    {                                                                                              \
        _DoublyListBase_PushBack((_DoublyListBase*)self, &val);                                    \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_PopBack(DoublyList_##T* self)                                    \
    {                                                                                              \
        _DoublyListBase_PopBack((_DoublyListBase*)self);                                           \
    }                                                                                              \
                                                                                                   \
    inline T* _DoublyList_##T##_Front(const DoublyList_##T* self)                                  \
    {                                                                                              \
        return (T*)_DoublyListBase_Front((const _DoublyListBase*)self);                            \
    }                                                                                              \
                                                                                                   \
    inline T* _DoublyList_##T##_Back(const DoublyList_##T* self)                                   \
    {                                                                                              \
        return (T*)_DoublyListBase_Back((const _DoublyListBase*)self);                             \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Insert(DoublyList_##T* self,                                     \
        DoublyList_##T##_Iterator pos, T val)                                                      \
    {                                                                                              \
        _DoublyListBase_InsertBefore((_DoublyListBase*)self, pos.node, &val);                      \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Erase(DoublyList_##T* self,                                      \
        DoublyList_##T##_Iterator pos)                                                             \
    {                                                                                              \
        _DoublyListBase_Erase((_DoublyListBase*)self, pos.node);                                   \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Clear(DoublyList_##T* self)                                      \
    {                                                                                              \
        _DoublyListBase_Clear((_DoublyListBase*)self);                                             \
    }                                                                                              \
                                                                                                   \
    inline bool _DoublyList_##T##_IsEmpty(const DoublyList_##T* self)                              \
    {                                                                                              \
        return _DoublyListBase_IsEmpty((const _DoublyListBase*)self);                              \
    }                                                                                              \
                                                                                                   \
    inline umax _DoublyList_##T##_Size(const DoublyList_##T* self)                                 \
    {                                                                                              \
        return _DoublyListBase_Size((const _DoublyListBase*)self);                                 \
    }                                                                                              \
                                                                                                   \
    inline void _DoublyList_##T##_Reverse(DoublyList_##T* self)                                    \
    {                                                                                              \
        _DoublyListBase_Reverse((_DoublyListBase*)self);                                           \
    }                                                                                              \
                                                                                                   \
    inline DoublyList_##T##_Iterator _DoublyList_##T##_Begin(const DoublyList_##T* self)           \
    {                                                                                              \
        DoublyList_##T##_Iterator it;                                                              \
        _DoublyList_##T##_Iterator##_Create(&it);                                                  \
        it.node = ((_DoublyListBase*)self)->head.next;                                             \
        it.list = (const _DoublyListBase*)self;                                                    \
        return it;                                                                                 \
    }                                                                                              \
                                                                                                   \
    inline DoublyList_##T##_Iterator _DoublyList_##T##_End(const DoublyList_##T* self)             \
    {                                                                                              \
        DoublyList_##T##_Iterator it;                                                              \
        _DoublyList_##T##_Iterator##_Create(&it);                                                  \
        it.node = (_DListNode*)&((_DoublyListBase*)self)->head;                                    \
        it.list = (const _DoublyListBase*)self;                                                    \
        return it;                                                                                 \
    }                                                                                              \
    /* ===== Sorting (merge sort) ===== */                                                   \
    static inline _DListNode* _DoublyList_##T##_Merge(_DListNode* a, _DListNode* b,          \
        int (*cmp)(const T*, const T*))                                                      \
    {                                                                                        \
        _DListNode dummy = { NULL, NULL };                                                   \
        _DListNode* tail = &dummy;                                                           \
        while (a && b) {                                                                     \
            T* da = _DList_Data(a);                                                          \
            T* db = _DList_Data(b);                                                          \
            if (cmp(da, db) <= 0) {                                                          \
                tail->next = a;                                                              \
                a->prev = tail;                                                              \
                a = a->next;                                                                 \
            } else {                                                                         \
                tail->next = b;                                                              \
                b->prev = tail;                                                              \
                b = b->next;                                                                 \
            }                                                                                \
            tail = tail->next;                                                               \
        }                                                                                    \
        if (a) {                                                                             \
            tail->next = a;                                                                  \
            a->prev = tail;                                                                  \
        } else if (b) {                                                                      \
            tail->next = b;                                                                  \
            b->prev = tail;                                                                  \
        }                                                                                    \
        /* fix prev of first node */                                                         \
        dummy.next->prev = NULL;                                                             \
        return dummy.next;                                                                   \
    }                                                                                        \
                                                                                             \
    static inline _DListNode* _DoublyList_##T##_MergeSort(_DListNode* head,                  \
        int (*cmp)(const T*, const T*))                                                      \
    {                                                                                        \
        if (!head || !head->next) return head;                                               \
        /* find middle using slow/fast pointers */                                           \
        _DListNode* slow = head;                                                             \
        _DListNode* fast = head->next;                                                       \
        while (fast && fast->next) {                                                         \
            slow = slow->next;                                                               \
            fast = fast->next->next;                                                         \
        }                                                                                    \
        _DListNode* mid = slow->next;                                                        \
        slow->next = NULL;                                                                   \
        if (mid) mid->prev = NULL;                                                           \
        _DListNode* left = _DoublyList_##T##_MergeSort(head, cmp);                           \
        _DListNode* right = _DoublyList_##T##_MergeSort(mid, cmp);                           \
        return _DoublyList_##T##_Merge(left, right, cmp);                                    \
    }                                                                                        \
                                                                                             \
    static inline void _DoublyList_##T##_Sort(DoublyList_##T* self,                          \
        int (*cmp)(const T*, const T*))                                                      \
    {                                                                                        \
        ERR_RET_NULL(self);                                                                  \
        if (self->size <= 1) return;                                                         \
        if (!cmp) cmp = (int (*)(const T*, const T*))self->cmp;                              \
        /* Detach nodes from sentinel */                                                     \
        _DListNode* head = self->head.next;                                                  \
        head->prev = NULL;                                                                   \
        _DListNode* last = self->head.prev;                                                  \
        last->next = NULL;                                                                   \
        /* Sort the detached list */                                                         \
        _DListNode* new_head = _DoublyList_##T##_MergeSort(head, cmp);                       \
        /* Rebuild circular sentinel */                                                      \
        _DListNode* new_tail = new_head;                                                     \
        while (new_tail && new_tail->next) new_tail = new_tail->next;                        \
        self->head.next = new_head;                                                          \
        if (new_head) new_head->prev = &self->head;                                          \
        self->head.prev = new_tail;                                                          \
        if (new_tail) new_tail->next = &self->head;                                          \
        else self->head.next = self->head.prev = &self->head;                                \
    }

#define _DLIST_IMPL_EX2(T, CONSTRUCT, DESTROY, COPY, CMP)  _DLIST_IMPL_EX1(T, CONSTRUCT, DESTROY, COPY, CMP)
#define _DLIST_IMPL_EX3(T, CONSTRUCT, DESTROY, COPY, CMP)  _DLIST_IMPL_EX2(T, CONSTRUCT, DESTROY, COPY, CMP)
#define DLIST_IMPL_EX(T, CONSTRUCT, DESTROY, COPY, CMP)    _DLIST_IMPL_EX3(T, CONSTRUCT, DESTROY, COPY, CMP)

/* For POD types: NULL callbacks → memcpy-based */
#define DLIST_IMPL(T)  DLIST_IMPL_EX(T, NULL, NULL, NULL, _##T##_cmp_default)

/* ============ Type Shortcut Macros ============ */

#define _DoublyList(T)           DoublyList_##T
#define DoublyList(T)            _DoublyList(T)
#define DList                    DoublyList
#define _DoublyListIter(T)       DoublyList_##T##_Iterator
#define DoublyListIter(T)        _DoublyListIter(T)
#define DListIter                DoublyListIter

/* ============ Pre-instantiated Types ============ */

DLIST_IMPL(i8);
DLIST_IMPL(i16);
DLIST_IMPL(i32);
DLIST_IMPL(i64);
DLIST_IMPL(imax);
DLIST_IMPL(u8);
DLIST_IMPL(u16);
DLIST_IMPL(u32);
DLIST_IMPL(u64);
DLIST_IMPL(umax);
DLIST_IMPL(f32);
DLIST_IMPL(f64);
DLIST_IMPL(Object);

DLIST_IMPL_EX(String, _String_Create, _String_Destroy, _String_Copy, _String_cmp_default);
