#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include <z_map.h>
#include <z_array.h>

typedef enum {
  JSON_OBJECT,
  JSON_STRING,
  JSON_NUMBER,
  JSON_ARRAY,
} Json_Item_Type;

typedef struct Json_Item Json_Item;

Z_DEFINE_ARRAY(Json_Item_Array, Json_Item *);

struct Json_Item {
    Json_Item_Type type;
    union {
        Z_Map key_value_pairs;
        Z_String_View string;
        double number;
        Json_Item_Array array;
    };
};

Json_Item *json_parse(Z_Heap *heap, Token_Array tokens);

#endif
