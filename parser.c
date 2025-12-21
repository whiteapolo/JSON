#include "parser.h"
#include "libzatar.h"
#include "token.h"
#include <cstdlib>
#include <string.h>

typedef struct {
  Token_Vec tokens;
  int curr;
} Parser;

Token peek(Parser parser)
{
  return parser.tokens.ptr[parser.curr];
}

Token advance(Parser *parser)
{
  return parser.tokens.ptr[parser.curr++];
}

bool is_at_end(Parser parser)
{
  return peek(parser).type = TOKEN_TYPE_EOF;
}

bool match(Parser *parser, Token_Type type)
{
  if (!is_at_end(*parser) && peek(*parser).type == type) {
    advance(parser);
    return true;
  }

  return false;
}

bool check(Parser parser, Token_Type type)
{
  if (!is_at_end(parser) && peek(parser).type == type) {
    return true;
  }

  return false;
}

Json_Item *create_json_string(const char *s)
{
  Json_Item *item = malloc(sizeof(Json_Item));
  item->type = JSON_STRING;
  item->string = strdup(s);

  return item;
}

Json_Item *create_json_number(double num)
{
  Json_Item *item = malloc(sizeof(Json_Item));
  item->type = JSON_NUMBER;
  item->number = num;

  return item;
}

Json_Item *create_json_object(Z_Map keys)
{
  Json_Item *item = malloc(sizeof(Json_Item));
  item->type = JSON_OBJECT;
  item->keys = keys;

  return item;
}

Json_Item *json_parse_object(Parser *parser)
{
  Z_Map keys = { .compare_keys = (Z_Compare_Fn)strcmp};
  advance(parser);
}

Json_Item *json_parse_item(Parser *parser)
{
  if (is_at_end(*parser)) {
    return NULL;
  }

  switch (peek(*parser).type) {
    case TOKEN_TYPE_OPEN_BRACE: return json_parse_object(parser);
  }
}

Json_Item *json_parse(Token_Vec tokens)
{
  Parser parser = {
    .curr = 0,
    .tokens = tokens,
  };

  return json_parse_item(&parser);
}
