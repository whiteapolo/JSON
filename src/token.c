#include "token.h"
#include <stdio.h>

const char *token_kind_to_string(Json_Token_Kind kind) {
    switch (kind) {
        case TOKEN_KIND_EOF:           return "EOF";
        case TOKEN_KIND_COMMA:         return "COMMA";
        case TOKEN_KIND_COLON:         return "COLON";
        case TOKEN_KIND_ERROR:         return "ERROR";
        case TOKEN_KIND_NUMBER:        return "NUMBER";
        case TOKEN_KIND_STRING:        return "STRING";
        case TOKEN_KIND_OPEN_BRACE:    return "OPEN_BRACE";
        case TOKEN_KIND_CLOSE_BRACE:   return "CLOSE_BRACE";
        case TOKEN_KIND_OPEN_BRACKET:  return "OPEN_BRACKET";
        case TOKEN_KIND_CLOSE_BRACKET: return "CLOSE_BRACKET";
        case TOKEN_KIND_BOOLEAN:       return "BOOLEAN";
        default:                       return "UNKNOWN";
    }
}

void json_print_token(Json_Token token)
{
    printf(
            "{ kind: \"%s\", lexeme: \"%.*s\", number_value: %lf, line: %zu, col: %zu }\n",
            token_kind_to_string(token.kind),
            (int)token.lexeme.length,
            token.lexeme.ptr,
            token.number,
            token.line,
            token.column
          );
}

void json_print_tokens(Json_Token_Array tokens)
{
    for (size_t i = 0; i < tokens.length; i++) {
        json_print_token(tokens.ptr[i]);
    }
}
