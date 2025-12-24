#ifndef TOKEN_H
#define TOKEN_H

#include <z_string.h>

typedef enum {
  TOKEN_KIND_OPEN_BRACE,
  TOKEN_KIND_CLOSE_BRACE,
  TOKEN_KIND_OPEN_BRACKET,
  TOKEN_KIND_CLOSE_BRACKET,
  TOKEN_KIND_COMMA,
  TOKEN_KIND_COLON,
  TOKEN_KIND_STRING,
  TOKEN_KIND_NUMBER,
  TOKEN_KIND_ERROR,
  TOKEN_KIND_EOF,
} Json_Token_Kind;

typedef struct {
  Json_Token_Kind kind;
  size_t line;
  size_t column;
  Z_String_View lexeme;
  double number_value;
} Json_Token;

Z_DEFINE_ARRAY(Json_Token_Array, Json_Token);

Json_Token json_token_create(Json_Token_Kind kind, Z_String_View lexeme, size_t line, size_t column, double number_value);
Json_Token json_token_create_eof(size_t line, size_t column);
void json_print_tokens(Json_Token_Array tokens);
void json_print_token(Json_Token token);

#endif
