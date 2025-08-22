#ifndef LEXEL_H
#define LEXEL_H

#include "token.h"
typedef struct {
  Z_String_View source;
  int line;
  int column;
  int start;
  int end;
} Lexer;

Lexer create_lexer(Z_String_View source);
Token lexer_next(Lexer *lexer);
Token lexer_peek(Lexer lexer);

#endif
