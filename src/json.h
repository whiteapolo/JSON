#ifndef JSON_H
#define JSON_H

#include "../zlib/include/array.h"
#include "../zlib/include/string.h"
#include "../zlib/include/hash-table.h"

typedef enum {
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL,
    JSON_OBJECT,
    JSON_ARRAY,
} Json_Value_Type;

typedef struct Json_Value Json_Value;

Z_DEFINE_ARRAY(Json_Value_Array, Json_Value);

struct Json_Value {
    Json_Value_Type type;
    union {
        Z_String string;
        double number;
        bool boolean;
        Z_Hash_Table object;
        Json_Value_Array array;
    };
};

typedef struct {
    bool had_error;
    Json_Value json;
    char *error;
} Json_Parse_Result;

Json_Parse_Result json_parse_from_file(Z_Pool *pool, const char *pathname);
Json_Parse_Result json_parse_file(Z_Pool *pool, const char *pathname);

#endif
