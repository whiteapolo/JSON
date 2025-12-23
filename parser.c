#include "parser.h"
#include "libzatar.h"
#include "token.h"
#include <string.h>

typedef struct {
  Token_Vec tokens;
  int curr;
  bool had_error;
} Parser_State;

Json_Item *json_parse_object(Parser_State *parser);
Json_Item *json_parse_value(Parser_State *parser);

Parser_State create_parser(Token_Vec tokens)
{
  Parser_State parser = {
    .curr = 0,
    .had_error = false,
    .tokens = tokens,
  };

  return parser;
}

Json_Item *create_json_item_object(Z_Map key_value_pairs)
{
  Json_Item *json_item = malloc(sizeof(Json_Item));
  json_item->type = JSON_OBJECT;
  json_item->key_value_pairs = key_value_pairs;

  return json_item;
}

Json_Item *create_json_item_number(double number)
{
  Json_Item *json_item = malloc(sizeof(Json_Item));
  json_item->type = JSON_NUMBER;
  json_item->number = number;

  return json_item;
}

Json_Item *create_json_item_string(Z_String_View s)
{
  Json_Item *json_item = malloc(sizeof(Json_Item));
  json_item->type = JSON_STRING;
  json_item->string = z_sv_to_cstr(s);

  return json_item;
}

Json_Item *create_json_item_array(Json_Item_Array array)
{
  Json_Item *json_item = malloc(sizeof(Json_Item));
  json_item->type = JSON_ARRAY;
  json_item->array = array;

  return json_item;
}

Token peek(Parser_State parser)
{
  return parser.tokens.ptr[parser.curr];
}

Token advance(Parser_State *parser)
{
  return parser->tokens.ptr[parser->curr++];
}

bool is_at_end(Parser_State parser)
{
  return peek(parser).type == TOKEN_TYPE_EOF;
}

bool match(Parser_State *parser, Token_Type type)
{
  if (parser->had_error) {
    return false;
  }

  if (!is_at_end(*parser) && peek(*parser).type == type) {
    advance(parser);
    return true;
  }

  return false;
}

Token previous(Parser_State parser)
{
  return parser.tokens.ptr[parser.curr - 1];
}

bool check(Parser_State parser, Token_Type type)
{
  if (parser.had_error) {
    return false;
  }

  if (!is_at_end(parser) && peek(parser).type == type) {
    return true;
  }

  return false;
}

// JSON:                      OBJECT
// OBJECT:                    '{' OBJECT_KEY_VALUE_PAIRS? '}'
// OBJECT_KEY_VALUE_PAIRS:    KEY ':' VALUE (',' OBJECT_KEY_VALUE_PAIRS)*
// KEY:                       STRING
// VALUE:                     STRING | NUMBER | ARRAY | OBJECT
// ARRAY:                     '[' ARRAY_ITEMS? ']'
// ARRAY_ITEMS:               VALUE (',' VALUE)*

void parse_error(Parser_State *parser, const char *what_parser_expected)
{
  Token found = peek(*parser);
  advance(parser);
  parser->had_error = true;
  printf("%s, But found: '%.*s'", what_parser_expected, found.lexeme.len, found.lexeme.ptr);
}

Json_Item *json_parse_array(Parser_State *parser)
{
  if (!match(parser, TOKEN_TYPE_OPEN_BRACKET)) {
    parse_error(parser, "Expected '['");
    return NULL;
  }

  Json_Item_Array array = {0};

  do {
    z_da_append(&array, json_parse_value(parser));
  } while (match(parser, TOKEN_TYPE_COMMA));

  if (!match(parser, TOKEN_TYPE_CLOSE_BRACKET)) {
    parse_error(parser, "Expected ']'");
    return NULL;
  }

  return create_json_item_array(array);
}

Json_Item *json_parse_value(Parser_State *parser)
{
  if (check(*parser, TOKEN_TYPE_OPEN_BRACE)) {
    return json_parse_object(parser);
  }

  if (match(parser, TOKEN_TYPE_NUMBER)) {
    return create_json_item_number(previous(*parser).number_value);
  }

  if (match(parser, TOKEN_TYPE_STRING)) {
    return create_json_item_string(previous(*parser).lexeme);
  }

  if (check(*parser, TOKEN_TYPE_OPEN_BRACKET)) {
    return json_parse_array(parser);
  }

  parse_error(parser, "Expected: object | string | number | array");
  return NULL;
}

void json_parse_key_value_pairs(Parser_State *parser, Z_Map *key_value_pairs)
{
  if (check(*parser, TOKEN_TYPE_CLOSE_BRACE)) {
    return;
  }

  if (!check(*parser, TOKEN_TYPE_STRING)) {
    parse_error(parser, "Expected key");
    return;
  }

  Z_String_View key = advance(parser).lexeme;

  if (!match(parser, TOKEN_TYPE_COLON)) {
    parse_error(parser, "Expected ':'");
    return;
  }

  Json_Item *value = json_parse_value(parser);
  z_map_put(key_value_pairs, z_sv_to_cstr(key), value, NULL, NULL);

  if (match(parser, TOKEN_TYPE_COMMA)) {
    json_parse_key_value_pairs(parser, key_value_pairs);
  }
}

Json_Item *json_parse_object(Parser_State *parser)
{
  if (!match(parser, TOKEN_TYPE_OPEN_BRACE)) {
    parse_error(parser, "Expected '{'");
    return NULL;
  }

  Z_Map key_value_pairs = { .compare_keys = (Z_Compare_Fn)strcmp };

  json_parse_key_value_pairs(parser, &key_value_pairs);

  if (!match(parser, TOKEN_TYPE_CLOSE_BRACE)) {
    parse_error(parser, "Expected '}'");
    return NULL;
  }

  return create_json_item_object(key_value_pairs);
}

Json_Item *json_parse(Token_Vec tokens)
{
  Parser_State parser = create_parser(tokens);
  return json_parse_object(&parser);
}