#include "print_json.h"

void print_json_key_value(Z_Map map);
void print_json_value(Json_Item *value);
void print_json_object(Json_Item *json);

void print_json_array(Json_Item_Array array)
{
  if (array.len == 0) {
    printf("[]");
    return;
  }

  printf("[ ");
  for (int i = 0; i < array.len - 1; i++) {
    print_json_value(array.ptr[i]);
    printf(", ");
  }

  print_json_value(array.ptr[array.len - 1]);

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
      printf("\"%s\"", value->string);
      break;
    case JSON_ARRAY:
      print_json_array(value->array);
      break;
    case JSON_OBJECT:
      print_json_object(value);
      break;
  }
}

void print_key_value_pairs(void *key, void *value, void *arg)
{
  (void)arg;
  printf("\"%s\": ", (char *)key);
  print_json_value(value);
  printf(", ");
}

void print_json_key_value(Z_Map map)
{
  z_map_order_traverse(&map, print_key_value_pairs, NULL);
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
