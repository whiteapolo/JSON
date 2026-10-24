#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include "../zlib/include/scanner.h"

Json_Token lexer_capture(const Z_Scanner *scanner, Json_Token_Kind kind)
{
    Json_Token token = {
        .kind = kind,
        .lexeme = z_scanner_capture(scanner),
        .line = scanner->line,
        .column = scanner->column,
    };

    return token;
}

Json_Token lexer_capture_number(const Z_Scanner *scanner, double value)
{
    Json_Token token = {
        .kind = TOKEN_KIND_NUMBER,
        .lexeme = z_scanner_capture(scanner),
        .line = scanner->line,
        .column = scanner->column,
        .number = value,
    };

    return token;
}

Json_Token lexer_capture_boolean(const Z_Scanner *scanner, bool value)
{
    Json_Token token = {
        .kind = TOKEN_KIND_BOOLEAN,
        .lexeme = z_scanner_capture(scanner),
        .line = scanner->line,
        .column = scanner->column,
        .boolean = value,
    };

    return token;
}

Json_Token lexer_capture_eof(const Z_Scanner *scanner)
{
    Json_Token token = {
        .kind = TOKEN_KIND_EOF,
        .line = scanner->line,
        .column = scanner->column,
    };

    return token;
}

Json_Token lexer_capture_error(const Z_Scanner *scanner, const char *expected)
{
    Json_Token token = lexer_capture(scanner, TOKEN_KIND_ERROR);

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

    Json_Token token = lexer_capture(scanner, TOKEN_KIND_STRING);

    if (!z_scanner_match(scanner, '"')) {
        return lexer_capture_error(scanner, "Expected '\"' at line end");
    }

    return token;
}

bool is_number_character(char c)
{
    return isdigit(c) || strchr(".e-+", c);
}

bool string_to_number(Z_String_View s, double *result)
{
    char buf[s.length + 1];
    strncpy(buf, s.ptr, s.length + 1);

    char *nptr = buf;
    char *endptr = NULL;
    double value = strtod(nptr, &endptr);

    if (nptr == endptr) {
        return false;
    }

    *result = value;
    return true;
}

Json_Token lexer_number(Z_Scanner *scanner)
{
    while (!z_scanner_is_at_end(scanner) && is_number_character(z_scanner_peek(scanner))) {
        z_scanner_advance(scanner, 1);
    }

    Z_String_View lexeme = z_scanner_capture(scanner);
    double value;

    if (!string_to_number(lexeme, &value)) {
        return lexer_capture_error(scanner, "Expected number");
    }

    return lexer_capture_number(scanner, value);
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
        return lexer_capture_eof(scanner);
    }

    if (z_scanner_match_string(scanner, z_sv("null"))) {
        return lexer_capture(scanner, TOKEN_KIND_NULL);
    }

    if (z_scanner_match_string(scanner, z_sv("false"))) {
        return lexer_capture_boolean(scanner, false);
    }

    if (z_scanner_match_string(scanner, z_sv("true"))) {
        return lexer_capture_boolean(scanner, true);
    }

    char c = z_scanner_peek(scanner);
    z_scanner_advance(scanner, 1);

    if (is_number_character(c)) {
        return lexer_number(scanner);
    }

    switch (c) {
        case ',': return lexer_capture(scanner, TOKEN_KIND_COMMA);
        case '{': return lexer_capture(scanner, TOKEN_KIND_OPEN_BRACE);
        case '}': return lexer_capture(scanner, TOKEN_KIND_CLOSE_BRACE);
        case '[': return lexer_capture(scanner, TOKEN_KIND_OPEN_BRACKET);
        case ']': return lexer_capture(scanner, TOKEN_KIND_CLOSE_BRACKET);
        case ':': return lexer_capture(scanner, TOKEN_KIND_COLON);
        case '"': return lexer_string(scanner);
        default: return lexer_unknown(scanner);
    }
}

Json_Lexer_Result json_lex(Z_Pool *pool, Z_String_View source)
{
    Json_Lexer_Result result = {
        .ok = true,
        .tokens = z_array_new(pool, Json_Token_Array),
    };

    Z_Scanner scanner = z_scanner_new(source);
    Json_Token token = lexer_next(&scanner);

    while (token.kind != TOKEN_KIND_EOF) {

        if (token.kind == TOKEN_KIND_ERROR) {
            result.ok = false;
        }

        z_array_push_back(&result.tokens, token);
        token = lexer_next(&scanner);
    }

    z_array_push_back(&result.tokens, token);
    return result;
}
