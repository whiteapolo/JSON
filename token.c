#include "token.h"
#include <stdio.h>

Json_Token create_token(Json_Token_Kind kind, Z_String_View lexeme, size_t line, size_t column, double number_value)
{
  Json_Token token = {
    .kind = kind,
    .lexeme = lexeme,
    .line = line,
    .column = column,
    .number_value = number_value,
  };

  return token;
}

Json_Token create_eof_token(size_t line, size_t column)
{
  Json_Token token = {
    .kind = TOKEN_KIND_EOF,
    .line = line,
    .column = column,
  };

  return token;
}

const char *token_kind_to_string(Json_Token_Kind kind) {
    switch (kind) {
        case TOKEN_KIND_EOF:           return "EOF";
        case TOKEN_KIND_COMMA:         return "COMMA";
        case TOKEN_KIND_COLON:         return "COLON";
        case TOKEN_KIND_ERROR:         return "ERROR";
        case TOKEN_KIND_NUMBER:        return "NUMBER";
        case TOKEN_KIND_STRING:        return "STRING";
        case TOKEN_KIND_OPEN_BRACE:    return "OPEN_BRACE";
        case TOKEN_KIND_CLOSE_BRACE:   return "CLOSE_BRACE";
        case TOKEN_KIND_OPEN_BRACKET:  return "OPEN_BRACKET";
        case TOKEN_KIND_CLOSE_BRACKET: return "CLOSE_BRACKET";
        default:                       return "UNKNOWN";
    }
}

void print_token(Json_Token token)
{
  printf(
      "{ kind: \"%s\", lexeme: \"%.*s\", number_value: %lf, line: %zu, col: %zu }\n",
      token_kind_to_string(token.kind),
      (int)token.lexeme.length,
      token.lexeme.ptr,
      token.number_value,
      token.line,
      token.column
  );
}

void print_tokens(Json_Token_Array tokens)
{
  for (size_t i = 0; i < tokens.length; i++) {
    print_token(tokens.ptr[i]);
  }
}
