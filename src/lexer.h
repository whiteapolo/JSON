#ifndef LEXEL_H
#define LEXEL_H

#include "token.h"

typedef struct {
  Json_Token_Array tokens;
  bool ok;
} Json_Lexer_Result;

Json_Lexer_Result json_lex(Z_Heap *heap, Z_String_View source);

#endif
