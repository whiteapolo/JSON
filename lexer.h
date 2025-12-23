#ifndef LEXEL_H
#define LEXEL_H

#include "token.h"

typedef struct {
  Token_Array tokens;
  bool had_errors;
} Lexer_Result;

Lexer_Result lex(Z_Heap *heap, Z_String_View source);

#endif
