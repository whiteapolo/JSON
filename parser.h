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
} Json_Value_Kind;

typedef struct Json_Value Json_Value;

Z_DEFINE_ARRAY(Json_Item_Array, Json_Value *);

struct Json_Value {
    Json_Value_Kind type;
    union {
        Z_Map key_value_pairs;
        Z_String_View string;
        double number;
        Json_Item_Array array;
    };
};

Json_Value *json_parse(Z_Heap *heap, Token_Array tokens);

#endif
