#include "print_json.h"
#include <stdio.h>
#include <z_map.h>

void print_json_key_value(Z_Map map);
void print_json_value(Json_Item *value);
void print_json_object(Json_Item *json);

void print_json_array(Json_Item_Array array)
{
  printf("[ ");
  for (size_t i = 0; i < array.length - 1; i++) {
    print_json_value(array.ptr[i]);
    if (i != array.length - 1) {
      printf(", ");
    }
  }
  printf("]");
}

void print_json_value(Json_Item *value)
{
  switch (value->type)
  {
    case JSON_NUMBER:
      printf("%lf", value->number);
      break;
    case JSON_STRING:
      printf("\"");
      z_sv_print(value->string);
      printf("\"");
      break;
    case JSON_ARRAY:
      print_json_array(value->array);
      break;
    case JSON_OBJECT:
      print_json_object(value);
      break;
  }
}

void print_json_key_value(Z_Map map)
{
  Z_Heap_Auto heap = {0};
  Z_Key_Value_Array pairs = z_map_to_array(&heap, &map);

  for (size_t i = 0; i < pairs.length; i++) {
    printf("\"%s\": ", (char *)pairs.ptr[i].key);
    print_json_value(pairs.ptr[i].value);
    if (i != pairs.length - 1) {
      printf(", ");
    }
  }
}

void print_json_object(Json_Item *json)
{
  printf("{ ");
  print_json_key_value(json->key_value_pairs);
  printf(" }");
}

void print_json(Json_Item *json)
{
  print_json_object(json);
  printf("\n");
}
