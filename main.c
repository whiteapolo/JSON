#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "print_json.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define LIBZATAR_IMPLEMENTATION
#include "libzatar.h"

int main(void) {
  Z_String f = {0};
  z_read_whole_file("./data.json", &f);

  Token_Vec tokens = lex(Z_STR(f));
  Json_Item *json = json_parse(tokens);

  print_json(json);

  return 0;
}
