#include "lexer.h"
#include "token.h"
#define LIBZATAR_IMPLEMENTATION
#include "libzatar.h"

#include <stdbool.h>
#include <stdio.h>

int main(void)
{
  Z_String f = {0};
  z_read_whole_file("./data.json", &f);
  z_sv_print(Z_STR(f));

  Token_Vec tokens = lex(Z_STR(f));
  print_tokens(tokens);

  return 0;
}
