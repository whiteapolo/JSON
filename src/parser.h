#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "../zlib/include/array.h"
#include "../zlib/include/string.h"
#include "../zlib/include/hash-table.h"

typedef enum {
    JSON_VALUE_OBJECT,
    JSON_VALUE_STRING,
    JSON_VALUE_NUMBER,
    JSON_VALUE_BOOLEAN,
    JSON_VALUE_ARRAY,
    JSON_VALUE_NULL,
} Json_Value_Kind;

typedef struct Json_Value Json_Value;

Z_DEFINE_ARRAY(Json_Value_Array, Json_Value);

struct Json_Value {
    Json_Value_Kind kind;
    union {
        Z_Hash_Table object;
        Z_String string;
        Json_Value_Array array;
        double number;
        bool boolean;
    };
};

Json_Value *json_parse(Z_Pool *pool, Z_String_View document);

#endif
