#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "print_json.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <z_string.h>
#include <z_file.h>

int main(void)
{
  Z_Heap_Auto heap = {0};
  Z_Maybe_String result = z_read_file(&heap, "./5MB.json");

  if (!result.ok) {
    return 1;
  }

  Json_Lexer_Result lexer_result = json_lex(&heap, z_sv_from_str(&result.value));

  if (!lexer_result.ok) {
    return 2;
  }

  Json_Value *json = json_parse(&heap, lexer_result.tokens);
  print_json(json);

  return 0;
}
