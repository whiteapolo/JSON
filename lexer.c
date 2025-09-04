#include "lexer.h"
#include "libzatar.h"
#include "token.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

bool issign(char c)
{
  return c == '-' || c == '+';
}

Lexer create_lexer(Z_String_View source)
{
  return z_scanner_new(source);
}

Token lexer_capture_token(Lexer lexer, Token_Type type)
{
  return create_token(type, z_scanner_capture(lexer), lexer.line, lexer.column);
}

Token lexer_capture_error(Lexer lexer)
{
  return lexer_capture_token(lexer, TOKEN_TYPE_ERROR);
}

Token lexer_string(Lexer *lexer)
{
  z_scanner_reset_mark(lexer);

  while (!z_scanner_is_at_end(*lexer) && !z_scanner_check(*lexer, '"')) {
    z_scanner_advance(lexer);
  }

  Token token = lexer_capture_token(*lexer, TOKEN_TYPE_STRING);

  if (!z_scanner_match(lexer, '"')) {
    return lexer_capture_error(*lexer);
  }

  return token;
}

Token lexer_number(Lexer *lexer)
{
  double num = z_scanner_match_number(lexer);

  if (isnan(num)) {
    return lexer_capture_error(*lexer);
  }

  return lexer_capture_token(*lexer, TOKEN_TYPE_NUMBER);
}

Token lexer_next(Lexer *lexer)
{
  z_scanner_skip_spaces(lexer);
  z_scanner_reset_mark(lexer);

  if (z_scanner_is_at_end(*lexer)) {
    return create_eof_token(lexer->line, lexer->column);
  }

  if (isdigit(z_scanner_peek(*lexer)) || issign(z_scanner_peek(*lexer))) {
      return lexer_number(lexer);
  }

  switch (z_scanner_advance(lexer)) {
    case ',': return lexer_capture_token(*lexer, TOKEN_TYPE_COMMA);
    case '{': return lexer_capture_token(*lexer, TOKEN_TYPE_OPEN_BRACE);
    case '}': return lexer_capture_token(*lexer, TOKEN_TYPE_CLOSE_BRACE);
    case '[': return lexer_capture_token(*lexer, TOKEN_TYPE_OPEN_BRACKET);
    case ']': return lexer_capture_token(*lexer, TOKEN_TYPE_CLOSE_BRACKET);
    case ':': return lexer_capture_token(*lexer, TOKEN_TYPE_COLON);
    case '"': return lexer_string(lexer);
    default: return lexer_capture_error(*lexer);
  }
}

Token_Vec lex(Z_String_View source)
{
  Lexer lexer = create_lexer(source);
  Token_Vec tokens = {0};

  Token token = lexer_next(&lexer);

  while (token.type != TOKEN_TYPE_EOF) {

    if (token.type == TOKEN_TYPE_ERROR) {
      tokens.had_errors = true;
    }

    z_da_append(&tokens, token);
    token = lexer_next(&lexer);
  }

  z_da_append(&tokens, token);

  return tokens;
}
