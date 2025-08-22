#include "lexer.h"
#include "token.h"
#include <string.h>
#include <stdbool.h>

Lexer create_lexer(Z_String_View source)
{
  Lexer lexer = {
    .source = source,
    .start = 0,
    .end = 0,
    .line = 1,
    .column = 1,
  };

  return lexer;
}

char lexer_advance(Lexer *lexer)
{
  char c = lexer.source.ptr[lexer.end++];
  lexer->column++;

  if (c == '\n') {
    lexer->column = 1;
    lexer->line++;
  }

  return c;
}

char lexer_peek(Lexer lexer)
{
  return lexer.source.ptr[lexer.end];
}

bool lexer_is_at_end(Lexer lexer)
{
  return lexer.end >= lexer.source.len;
}

void lexer_skip_spaces(Lexer *lexer)
{
  while (!lexer_is_at_end(*lexer) && strchr("\n\t\r ", lexer_peek(*lexer))) {
    lexer_advance(lexer);
  }
}

Token lexer_next(Lexer *lexer)
{
  lexer_skip_spaces(lexer);
  lexer->start = lexer->end;

  if (lexer_is_at_end(*lexer)) {
    return create_eof_token(lexer->line, lexer->column);
  }

  switch () {
  cases
  }

}

Token lexer_peek(Lexer lexer);
