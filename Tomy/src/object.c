#include "class/object_class.h"
#include "ustring.h"
#include "print.h"

String* _Object_ToString(Object* self)
{
    String* str = New(String, STRING_CAPACITY);
    Call(String, str, Append, "Object");
    #ifdef _ADDRESS_PRINT_
    char buf[TEMP_BUFFER_SIZE] = { 0 };
    const int len = snprintf(buf, sizeof(buf), " at %p", self);
    Call(String, str, AppendN, buf, len);
    #endif
    return str;
}