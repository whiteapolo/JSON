#include "print_json.h"
#include <stdio.h>

void print_json_value(Json_Value *value);
void print_json_object(Json_Value *json);

void print_json_array(Json_Value_Array array)
{
    printf("[ ");
    for (size_t i = 0; i < array.length; i++) {
        print_json_value(array.ptr[i]);
        if (i != array.length - 1) {
            printf(", ");
        }
    }
    printf("]");
}

void print_json_value(Json_Value *value)
{
    switch (value->kind)
    {
        case JSON_VALUE_KIND_NUMBER:
            printf("%lf", value->as.number);
            break;
        case JSON_VALUE_KIND_NULL:
            printf("null");
            break;
        case JSON_VALUE_KIND_STRING:
            printf("\"");
            z_sv_print(value->as.string);
            printf("\"");
            break;
        case JSON_VALUE_KIND_ARRAY:
            print_json_array(value->as.array);
            break;
        case JSON_VALUE_KIND_OBJECT:
            print_json_object(value);
            break;
    }
}

void print_json_object(Json_Value *json)
{
    Z_Pool_Auto pool = {0};

    if (z_hash_table_size(&json->as.key_value_pairs) == 0) {
        printf("{}");
        return;
    }

    Z_Pair pair;
    Z_Hash_Table_Iter iter = z_hash_table_iter(&json->as.key_value_pairs);

    size_t i = 1;

    printf("{");
    while (z_hash_table_iter_next(&iter, &pair)) {
        printf("\"%s\": ", (char *)pair.key);
        print_json_value(pair.value);

        if (i == z_hash_table_size(&json->as.key_value_pairs)) {
            printf(", ");
        }

        i++;
    }
    printf("}");
}

void print_json(Json_Value *json)
{
    print_json_value(json);
    printf("\n");
}
