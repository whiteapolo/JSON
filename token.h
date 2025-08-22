#ifndef TOKEN_H
#define TOKEN_H

#include "libzatar.h"

typedef enum {
  TOKEN_TYPE_OPEN_PAREN,
  TOKEN_TYPE_CLOSE_PAREN,
  TOKEN_TYPE_OPEN_BRACE,
  TOKEN_TYPE_CLOSE_BRACE,
  TOKEN_TYPE_OPEN_BRACKET,
  TOKEN_TYPE_CLOSE_BRACKET,
  TOKEN_TYPE_COMMA,
  TOKEN_TYPE_COLON,
  TOKEN_TYPE_STRING,
  TOKEN_TYPE_NUMBER,
  TOKEN_TYPE_NULL,
  TOKEN_TYPE_EOF,
} Token_Type;

typedef struct {
  Token_Type type;
  Z_String_View lexeme;
  int line;
  int column;
} Token;

Token create_token(Token_Type type, Z_String_View lexeme, int line, int column);
Token create_eof_token(int line, int column);

#endif
