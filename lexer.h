#ifndef LEXEL_H
#define LEXEL_H

#include "libzatar.h"
#include "token.h"

typedef Z_Scanner Lexer;

Token_Array lex(Z_String_View source);

#endif
