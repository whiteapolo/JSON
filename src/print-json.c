#include "print-json.h"
#include <stdio.h>

void print_json_object(Json_Value *json, int level, int indentation);
void print_json_value(Json_Value *value, int level, int indentation);

void print_indentation(int level, int indentation)
{
    int spaces = level * indentation;
    for (int i = 0; i < spaces; i++) {
        printf("%c", ' ');
    }
}

void print_json_array(Json_Value_Array array, int level, int indentation)
{
    printf("[\n");

    for (size_t i = 0; i < array.length; i++) {
        print_indentation(level + 1, indentation);
        print_json_value(array.ptr[i], level + 1, indentation);

        if (i != array.length - 1) {
            printf(", ");
        }

        printf("\n");
    }

    print_indentation(level, indentation);
    printf("]");
}

void print_json_value(Json_Value *value, int level, int indentation)
{
    switch (value->kind)
    {
        case JSON_VALUE_KIND_NUMBER:
            printf("%lf", value->number);
            break;
        case JSON_VALUE_KIND_BOOLEAN:
            printf(value->boolean ? "true" : "false");
            break;
        case JSON_VALUE_KIND_NULL:
            printf("null");
            break;
        case JSON_VALUE_KIND_STRING:
            printf("\"");
            z_sv_print(value->string);
            printf("\"");
            break;
        case JSON_VALUE_KIND_ARRAY:
            print_json_array(value->array, level, indentation);
            break;
        case JSON_VALUE_KIND_OBJECT:
            print_json_object(value, level, indentation);
            break;
    }
}

void print_json_object(Json_Value *json, int level, int indentation)
{
    Z_Pool_Auto pool = {0};

    if (z_hash_table_size(&json->record) == 0) {
        printf("{}");
        return;
    }

    Z_Pair pair;
    Z_Hash_Table_Iter iter = z_hash_table_iter(&json->record);

    size_t i = 1;

    printf("{\n");

    while (z_hash_table_iter_next(&iter, &pair)) {
        print_indentation(level + 1, indentation);
        printf("\"%s\": ", (char *)pair.key);
        print_json_value(pair.value, level + 1, indentation);

        if (i != z_hash_table_size(&json->record)) {
            printf(",");
        }

        printf("\n");
        i++;
    }

    print_indentation(level, indentation);
    printf("}");
}

void print_json(Json_Value *json)
{
    print_json_value(json, 0, 4);
    printf("\n");
}
