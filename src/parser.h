#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "../zlib/include/array.h"
#include "../zlib/include/hash-table.h"

typedef enum {
    JSON_VALUE_KIND_OBJECT,
    JSON_VALUE_KIND_STRING,
    JSON_VALUE_KIND_NUMBER,
    JSON_VALUE_KIND_BOOLEAN,
    JSON_VALUE_KIND_ARRAY,
    JSON_VALUE_KIND_NULL,
} Json_Value_Kind;

typedef struct Json_Value Json_Value;

Z_DEFINE_ARRAY(Json_Value_Array, Json_Value *);

struct Json_Value {
    Json_Value_Kind kind;
    union {
        Z_Hash_Table record;
        Z_String_View string;
        Json_Value_Array array;
        double number;
        bool boolean;
    };
};

Json_Value *json_parse(Z_Pool *pool, Json_Token_Array tokens);

#endif
