#include "token.h"

Token create_token(Token_Type type, Z_String_View lexeme, int line, int column)
{
  Token token = {
    .type = type,
    .lexeme = lexeme,
    .line = line,
    .column = column,
  };

  return token;
}

Token create_eof_token(int line, int column)
{
  Token token = {
    .type = TOKEN_TYPE_EOF,
    .line = line,
    .column = column,
  };

  return token;
}
