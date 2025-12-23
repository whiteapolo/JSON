#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <z_scanner.h>

Token lexer_capture_token(const Z_Scanner *scanner, Token_Type type)
{
  return create_token(type, z_scanner_capture(scanner), scanner->line, scanner->column, 0);
}

Token lexer_capture_number_token(const Z_Scanner *scanner, double number_value)
{
  return create_token(TOKEN_TYPE_NUMBER, z_scanner_capture(scanner), scanner->line, scanner->column, number_value);
}

Token lexer_capture_error(const Z_Scanner *scanner)
{
  return lexer_capture_token(scanner, TOKEN_TYPE_ERROR);
}

Token lexer_string(Z_Scanner *scanner)
{
  z_scanner_reset_mark(scanner);
  z_scanner_advance_until(scanner, '"');

  Token token = lexer_capture_token(scanner, TOKEN_TYPE_STRING);

  if (!z_scanner_match(scanner, '"')) {
    return lexer_capture_error(scanner);
  }

  return token;
}

bool is_number_character(char c)
{
  return isdigit(c) || strchr(".e-+", c);
}

double string_to_number(Z_String_View s)
{
  char *nptr = strndup(s.ptr, s.length);
  char *endptr = NULL;
  double value = strtod(nptr, &endptr);

  if (nptr == endptr) {
    free(nptr);
    return NAN;
  }

  free(nptr);
  return value;
}

Token lexer_number(Z_Scanner *scanner)
{
  while (!z_scanner_is_at_end(scanner) && is_number_character(z_scanner_peek(scanner))) {
    z_scanner_advance(scanner, 1);
  }

  Z_String_View lexeme = z_scanner_capture(scanner);
  double value = string_to_number(lexeme);

  if (isnan(value)) {
    return lexer_capture_error(scanner);
  }

  return lexer_capture_number_token(scanner, value);
}

Token lexer_unknown(Z_Scanner *scanner)
{
  z_scanner_advance_until(scanner, '\n');
  return lexer_capture_error(scanner);
}

Token lexer_next(Z_Scanner *scanner)
{
  z_scanner_skip_spaces(scanner);
  z_scanner_reset_mark(scanner);

  if (z_scanner_is_at_end(scanner)) {
    return create_eof_token(scanner->line, scanner->column);
  }

  char c = z_scanner_peek(scanner);
  z_scanner_advance(scanner, 1);

  if (is_number_character(c)) {
    return lexer_number(scanner);
  }

  switch (c) {
    case ',': return lexer_capture_token(scanner, TOKEN_TYPE_COMMA);
    case '{': return lexer_capture_token(scanner, TOKEN_TYPE_OPEN_BRACE);
    case '}': return lexer_capture_token(scanner, TOKEN_TYPE_CLOSE_BRACE);
    case '[': return lexer_capture_token(scanner, TOKEN_TYPE_OPEN_BRACKET);
    case ']': return lexer_capture_token(scanner, TOKEN_TYPE_CLOSE_BRACKET);
    case ':': return lexer_capture_token(scanner, TOKEN_TYPE_COLON);
    case '"': return lexer_string(scanner);
    default: return lexer_unknown(scanner);
  }
}

Lexer_Result lex(Z_Heap *heap, Z_String_View source)
{
  Lexer_Result result = {
    .had_errors = false,
    .tokens = z_array_new(heap, Token_Array),
  };
  
  Z_Scanner scanner = z_scanner_new(source);
  Token token = lexer_next(&scanner);

  while (token.type != TOKEN_TYPE_EOF) {

    if (token.type == TOKEN_TYPE_ERROR) {
      result.had_errors = true;
    }

    z_array_push(&result.tokens, token);
    token = lexer_next(&scanner);
  }

  z_array_push(&result.tokens, token);
  return result;
}
