#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <z_scanner.h>

Json_Token lexer_capture_token(const Z_Scanner *scanner, Json_Token_Kind kind)
{
  return json_token_create(kind, z_scanner_capture(scanner), scanner->line, scanner->column, 0);
}

Json_Token lexer_capture_number_token(const Z_Scanner *scanner, double number_value)
{
  return json_token_create(TOKEN_KIND_NUMBER, z_scanner_capture(scanner), scanner->line, scanner->column, number_value);
}

Json_Token lexer_capture_error(const Z_Scanner *scanner, const char *expected)
{
  Json_Token token = lexer_capture_token(scanner, TOKEN_KIND_ERROR);
  printf(
    "%s, But found: '%.*s', At: %zu:%zu\n",
    expected,
    (int)token.lexeme.length,
    token.lexeme.ptr,
    token.line,
    token.column
  );
  return token;
}

Json_Token lexer_string(Z_Scanner *scanner)
{
  z_scanner_reset_mark(scanner);

  while (!z_scanner_is_at_end(scanner) && !z_scanner_check(scanner, '\n') && !z_scanner_check(scanner, '"')) {
    z_scanner_advance(scanner, 1);
  }

  Json_Token token = lexer_capture_token(scanner, TOKEN_KIND_STRING);

  if (!z_scanner_match(scanner, '"')) {
    return lexer_capture_error(scanner, "Expected '\"' at line end");
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

Json_Token lexer_number(Z_Scanner *scanner)
{
  while (!z_scanner_is_at_end(scanner) && is_number_character(z_scanner_peek(scanner))) {
    z_scanner_advance(scanner, 1);
  }

  Z_String_View lexeme = z_scanner_capture(scanner);
  double value = string_to_number(lexeme);

  if (isnan(value)) {
    return lexer_capture_error(scanner, "Expected number");
  }

  return lexer_capture_number_token(scanner, value);
}

Json_Token lexer_unknown(Z_Scanner *scanner)
{
  z_scanner_advance_until(scanner, '\n');
  return lexer_capture_error(scanner, "Expected anything");
}

Json_Token lexer_next(Z_Scanner *scanner)
{
  z_scanner_skip_spaces(scanner);
  z_scanner_reset_mark(scanner);

  if (z_scanner_is_at_end(scanner)) {
    return json_token_create_eof(scanner->line, scanner->column);
  }

  if (z_scanner_match_string(scanner, z_sv_from_cstr("null"))) {
    return lexer_capture_token(scanner, TOKEN_KIND_NULL);
  }

  char c = z_scanner_peek(scanner);
  z_scanner_advance(scanner, 1);

  if (is_number_character(c)) {
    return lexer_number(scanner);
  }

  switch (c) {
    case ',': return lexer_capture_token(scanner, TOKEN_KIND_COMMA);
    case '{': return lexer_capture_token(scanner, TOKEN_KIND_OPEN_BRACE);
    case '}': return lexer_capture_token(scanner, TOKEN_KIND_CLOSE_BRACE);
    case '[': return lexer_capture_token(scanner, TOKEN_KIND_OPEN_BRACKET);
    case ']': return lexer_capture_token(scanner, TOKEN_KIND_CLOSE_BRACKET);
    case ':': return lexer_capture_token(scanner, TOKEN_KIND_COLON);
    case '"': return lexer_string(scanner);
    default: return lexer_unknown(scanner);
  }
}

Json_Lexer_Result json_lex(Z_Heap *heap, Z_String_View source)
{
  Json_Lexer_Result result = {
    .ok = true,
    .tokens = z_array_new(heap, Json_Token_Array),
  };
  
  Z_Scanner scanner = z_scanner_new(source);
  Json_Token token = lexer_next(&scanner);

  while (token.kind != TOKEN_KIND_EOF) {

    if (token.kind == TOKEN_KIND_ERROR) {
      result.ok = false;
    }

    z_array_push(&result.tokens, token);
    token = lexer_next(&scanner);
  }

  z_array_push(&result.tokens, token);
  return result;
}
