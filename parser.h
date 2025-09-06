#ifndef PARSER_H
#define PARSER_H

#include "libzatar.h"
#include "token.h"

typedef enum {
  JSON_OBJECT,
  JSON_STRING,
  JSON_NUMBER,
} Json_Item_Type;

typedef struct Json_Item Json_Item;

typedef struct {
    Json_Item *ptr;
    int len;
    int cap;
} Json_Item_Array;

struct Json_Item {
    Json_Item_Type type;
    union {
        Z_Map keys;
        char *string;
        double number;
        Json_Item_Array array;
    };
};

Json_Item *json_parse(Token_Vec tokens);

#endif
