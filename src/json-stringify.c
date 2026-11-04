#include "json-stringify.h"
#include "../zlib/include/string.h"
#include "json.h"

typedef struct {
    int level;
    int spaces_each_level;
} Indentation;

void json_value_stringify(const Json_Value *value, Indentation indentation, Z_String *out);
void json_object_stringify(const Json_Value *json, Indentation indentation, Z_String *out);

Indentation level_up(Indentation indentation)
{
    Indentation new = {
        .level = indentation.level + 1,
        .spaces_each_level = indentation.spaces_each_level,
    };

    return new;
}

void indentation_stringify(Indentation indentation, Z_String *out)
{
    int spaces = indentation.level * indentation.spaces_each_level;

    for (int i = 0; i < spaces; i++) {
        z_str_push_back_char(out, ' ');
    }
}

void json_array_stringify(const Json_Value *value, Indentation indentation, Z_String *out)
{
    z_str_push_back_cstr(out, "[\n");
    Json_Value_Array array = value->array;

    for (size_t i = 0; i < array.length; i++) {
        indentation_stringify(level_up(indentation), out);
        json_value_stringify(&array.ptr[i], level_up(indentation), out);

        if (i != array.length - 1) {
            z_str_push_back_char(out, ',');
        }

        z_str_push_back_char(out, '\n');
    }

    indentation_stringify(indentation, out);
    z_str_push_back_char(out, ']');
}

void json_value_stringify(const Json_Value *value, Indentation indentation, Z_String *out)
{
    switch (value->type) {
        case JSON_NUMBER:
            z_str_push_back(out, "%g", value->number);
            break;
        case JSON_BOOLEAN:
            z_str_push_back(out, value->boolean ? "true" : "false");
            break;
        case JSON_NULL:
            z_str_push_back(out, "null");
            break;
        case JSON_STRING:
            z_str_push_back(out, "\"%s\"", value->string.ptr);
            break;
        case JSON_ARRAY:
            json_array_stringify(value, indentation, out);
            break;
        case JSON_OBJECT:
            json_object_stringify(value, indentation, out);
            break;
    }
}

void json_object_stringify(const Json_Value *json, Indentation indentation, Z_String *out)
{
    Z_Pool_Auto pool = {0};

    if (z_hash_table_size(&json->object) == 0) {
        z_str_push_back_cstr(out, "{}");
        return;
    }

    Z_Pair pair;
    Z_Hash_Table_Iter iter = z_hash_table_iter(&json->object);

    size_t i = 1;

    z_str_push_back_cstr(out, "{\n");

    while (z_hash_table_iter_next(&iter, &pair)) {
        indentation_stringify(level_up(indentation), out);
        z_str_push_back(out, "\"%s\": ", (char *)pair.key);
        json_value_stringify(pair.value, level_up(indentation), out);

        if (i != z_hash_table_size(&json->object)) {
            z_str_push_back_char(out, ',');
        }

        z_str_push_back_char(out, '\n');
        i++;
    }

    indentation_stringify(indentation, out);
    z_str_push_back_char(out, '}');
}

void json_stringify(const Json_Value *json, Z_String *out)
{
    Indentation indentation = {
        .level = 0,
        .spaces_each_level = 4,
    };

    json_value_stringify(json, indentation, out);
    z_str_push_back_char(out, '\n');
}
