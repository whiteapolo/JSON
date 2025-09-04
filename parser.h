#ifndef PARSER_H
#define PARSER_H

#include "libzatar.h"
typedef enum {
  JSON_OBJECT,
  JSON_STRING,
  JSON_NUMBER,
} Json_Value_Type;

typedef struct {
  Json_Value *ptr;
  int len;
  int cap;
} Json_Value_Array;

typedef struct {
  Json_Value_Type type;
  union {
    Z_Map keys;             // object
    char *string;           // string
    double number;          // number
    Json_Value_Array array; // array
  }
} Json_Value;

#endif
