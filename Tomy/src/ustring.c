#include "ustring.h"

#include <assert.h>
#include <string.h>

#include "error.h"

static _String_VTable _String_VTable_Instance = {
    _String_Create,
    _String_Destroy,
    _String_New,
    _String_Delete,
    _String_ToString,
    .reserve = _String_Reserve,
    .copy = _String_Copy,
    .append_n = _String_AppendN,
};

size_t calculate_capacity(size_t size)
{
    size_t capacity = 0;
    const size_t max = SIZE_MAX;
    if (size > 0)
    {
        capacity = 1;
        while (capacity < size)
        {
            if (capacity > max / 2)
            {
                return max;
            }
            capacity *= 2;
        }
    }
    return capacity;
}

void _String_Create(String* self)
{
    assert(self != NULL);
    Object_Create((Object*)self);
    ((Object*)self)->vptr = (_Object_VTable*)&_String_VTable_Instance;
    char* buf = (char*)calloc(STRING_CAPACITY + 1, sizeof(char));
    if (buf == NULL)
    {
        *self = (String){ 0 };
        return;
    }
    self->capacity = STRING_CAPACITY;
    self->size = 0;
    self->data = buf;
}

void _String_CreateN(String* self, size_t capacity)
{
    assert(self != NULL);
    if (capacity == STRING_CAPACITY)
    {
        _String_Create(self);
        return;
    }
    Object_Create((Object*)self);
    ((Object*)self)->vptr = (_Object_VTable*)&_String_VTable_Instance;
    char* buf = (char*)calloc(capacity + 1, sizeof(char));
    if (buf == NULL)
    {
        *self = (String){ 0 };
        return;
    }
    self->capacity = capacity;
    self->size = 0;
    self->data = buf;
}

void _String_Destroy(String* self)
{
    assert(self != NULL);
    free(self->data);
    *self = (String){ 0 };
}

String* _String_New(size_t capacity)
{
    if (!capacity)
        capacity = STRING_CAPACITY;
    String* self = (String*)malloc(sizeof(String));

    ERR_RET_V_COND_MSG(self == NULL, NULL, "String Memory allocation failed.");

    _String_CreateN(self, capacity);
    return self;
}

void _String_Delete(String* self)
{
    assert(self != NULL);
    _String_Destroy(self);
    free(self);
}

String* _String_ToString(String* self)
{
    String* str = New(String, STRING_CAPACITY);
    if (self->data)
        Call(String, str, AppendN, self->data, self->size);
    return str;
}

bool _String_Reserve(String* self, const size_t new_capacity)
{
    ERR_RET_V_NULL(self, false);
    char* buf = (char*)realloc(self->data, new_capacity + 1);
    if (buf == NULL)
    {
        return false;
    }
    if (new_capacity > self->capacity)
    {
        memset(buf + self->capacity + 1, 0, new_capacity - self->capacity);
    }
    else if (new_capacity < self->size)
    {
        buf[new_capacity] = 0;
        self->size = new_capacity;
    }
    self->capacity = new_capacity;
    self->data = buf;
    return true;
}

bool _String_Copy(String* self, const String* src)
{
    ERR_RET_V_NULL(self, false);
    ERR_RET_V_NULL(src, false);
    ERR_RET_V_COND(self == src, true);

    _String_Create(self);

    const size_t needed = src->size;

    if (needed == 0)
    {
        return true;
    }

    return _String_Insert(self, 0, src->data);
}

bool _String_InsertN(String* self, size_t pos, const char* s, size_t n)
{
    assert(self != NULL);
    if (pos > self->size)
    {
        pos = self->size;
    }
    size_t new_size = self->size + n;
    if (new_size > self->capacity)
    {
        size_t new_capacity = calculate_capacity(new_size);
        bool ok = _String_Reserve(self, new_capacity);
        if (!ok)
        {
            return false;
        }
    }
    memmove(self->data + pos + n, self->data + pos, self->size - pos);
    memcpy(self->data + pos, s, n);
    self->size = new_size;
    return true;
}

bool _String_Insert(String* self, size_t pos, const char* s)
{
    assert(self != NULL);
    size_t n = strlen(s);
    return _String_InsertN(self, pos, s, n);
}

bool _String_AppendN(String* self, const char* s, size_t n)
{
    assert(self != NULL);
    return _String_InsertN(self, self->size, s, n);
}

bool _String_Append(String* self, const char* s)
{
    assert(self != NULL);
    size_t n = strlen(s);
    return _String_AppendN(self, s, n);
}