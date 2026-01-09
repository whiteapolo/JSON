#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include <z_map.h>
#include <z_array.h>

typedef enum {
  JSON_VALUE_KIND_OBJECT,
  JSON_VALUE_KIND_STRING,
  JSON_VALUE_KIND_NUMBER,
  JSON_VALUE_KIND_ARRAY,
  JSON_VALUE_KIND_NULL,
} Json_Value_Kind;

typedef struct Json_Value Json_Value;

Z_DEFINE_ARRAY(Json_Value_Array, Json_Value *);

struct Json_Value {
    Json_Value_Kind kind;
    union {
        Z_Map key_value_pairs;
        Z_String_View string;
        double number;
        Json_Value_Array array;
    } as;
};

Json_Value *json_parse(Z_Heap *heap, Json_Token_Array tokens);

#endif
