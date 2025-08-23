#include "token.h"

Token create_token(Token_Type type, Z_String_View lexeme, int line, int column)
{
  Token token = {
    .type = type,
    .lexeme = lexeme,
    .line = line,
    .column = column,
  };

  return token;
}

Token create_eof_token(int line, int column)
{
  Token token = {
    .type = TOKEN_TYPE_EOF,
    .line = line,
    .column = column,
  };

  return token;
}

const char *token_type_to_string(Token_Type type) {
    switch (type) {
        case TOKEN_TYPE_OPEN_BRACE:    return "OPEN_BRACE";
        case TOKEN_TYPE_CLOSE_BRACE:   return "CLOSE_BRACE";
        case TOKEN_TYPE_OPEN_BRACKET:  return "OPEN_BRACKET";
        case TOKEN_TYPE_CLOSE_BRACKET: return "CLOSE_BRACKET";
        case TOKEN_TYPE_COMMA:         return "COMMA";
        case TOKEN_TYPE_COLON:         return "COLON";
        case TOKEN_TYPE_STRING:        return "STRING";
        case TOKEN_TYPE_NUMBER:        return "NUMBER";
        case TOKEN_TYPE_ERROR:         return "ERROR";
        case TOKEN_TYPE_EOF:           return "EOF";
        default:                       return "UNKNOWN";
    }
}

void print_token(Token token)
{
    printf("{ type: \"%s\", lexeme: \"", token_type_to_string(token.type));
    z_sv_print(token.lexeme);
    printf("\", line: %d, col: %d }\n", token.line, token.column);
}

void print_tokens(Token_Vec tokens)
{
  z_da_foreach(Token*, token, &tokens) {
    print_token(*token);
  }
}
