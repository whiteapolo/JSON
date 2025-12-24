#include "print_json.h"
#include <stdio.h>
#include <z_map.h>

void print_json_value(Json_Value *value);
void print_json_object(Json_Value *json);

void print_json_array(Json_Item_Array array)
{
  printf("[ ");
  for (size_t i = 0; i < array.length; i++) {
    print_json_value(array.ptr[i]);
    if (i != array.length - 1) {
      printf(", ");
    }
  }
  printf("]\n");
}

void print_json_value(Json_Value *value)
{
  switch (value->type)
  {
    case JSON_VALUE_KIND_NUMBER:
      printf("%lf", value->number);
      break;
    case JSON_VALUE_KIND_STRING:
      printf("\"");
      z_sv_print(value->string);
      printf("\"");
      break;
    case JSON_VALUE_KIND_ARRAY:
      print_json_array(value->array);
      break;
    case JSON_VALUE_KIND_OBJECT:
      print_json_object(value);
      break;
  }
}

void print_json_object(Json_Value *json)
{
  Z_Heap_Auto heap = {0};
  Z_Key_Value_Array pairs = z_map_to_array(&heap, &json->key_value_pairs);

  if (pairs.length == 0) {
    printf("{}\n");
    return;
  }

  printf("{\n");
  for (size_t i = 0; i < pairs.length; i++) {
    printf("\"%s\": ", (char *)pairs.ptr[i].key);
    print_json_value(pairs.ptr[i].value);
    if (i != pairs.length - 1) {
      printf(", ");
    }
  }
  printf("}\n");
}

void print_json(Json_Value *json)
{
  print_json_value(json);
}
