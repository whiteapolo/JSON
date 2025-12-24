#include "parser.h"
#include "token.h"
#include <string.h>
#include <stdio.h>

typedef struct {
  Z_Heap *heap;
  Token_Array tokens;
  int curr;
  bool had_error;
} Parser_State;

Json_Value *json_parse_object(Parser_State *parser);
Json_Value *json_parse_value(Parser_State *parser);

Parser_State create_parser(Z_Heap *heap, Token_Array tokens)
{
  Parser_State parser = {
    .heap = heap,
    .curr = 0,
    .had_error = false,
    .tokens = tokens,
  };

  return parser;
}

Json_Value *create_json_item_object(Parser_State *parser, Z_Map key_value_pairs)
{
  Json_Value *json_item = z_heap_malloc(parser->heap, sizeof(Json_Value));
  json_item->type = JSON_VALUE_KIND_OBJECT;
  json_item->key_value_pairs = key_value_pairs;

  return json_item;
}

Json_Value *create_json_item_number(Parser_State *parser, double number)
{
  Json_Value *json_item = z_heap_malloc(parser->heap, sizeof(Json_Value));
  json_item->type = JSON_VALUE_KIND_NUMBER;
  json_item->number = number;

  return json_item;
}

Json_Value *create_json_item_string(Parser_State *parser, Z_String_View s)
{
  Json_Value *json_item = z_heap_malloc(parser->heap, sizeof(Json_Value));
  json_item->type = JSON_VALUE_KIND_STRING;
  json_item->string = s;

  return json_item;
}

Json_Value *create_json_item_array(Parser_State *parser, Json_Item_Array array)
{
  Json_Value *json_item = z_heap_malloc(parser->heap, sizeof(Json_Value));
  json_item->type = JSON_VALUE_KIND_ARRAY;
  json_item->array = array;

  return json_item;
}

Token peek(const Parser_State *parser)
{
  return parser->tokens.ptr[parser->curr];
}

Token advance(Parser_State *parser)
{
  return parser->tokens.ptr[parser->curr++];
}

bool is_at_end(const Parser_State *parser)
{
  return peek(parser).type == TOKEN_TYPE_EOF;
}

bool match(Parser_State *parser, Token_Type type)
{
  if (parser->had_error) {
    return false;
  }

  if (!is_at_end(parser) && peek(parser).type == type) {
    advance(parser);
    return true;
  }

  return false;
}

Token previous(const Parser_State *parser)
{
  return parser->tokens.ptr[parser->curr - 1];
}

bool check(const Parser_State *parser, Token_Type type)
{
  if (parser->had_error || is_at_end(parser)) {
    return false;
  }

  return peek(parser).type == type;
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
  Token found = peek(parser);
  advance(parser);
  parser->had_error = true;
  printf(
      "%s, But found: '%.*s' At line: %zu col: %zu\n",
      what_parser_expected,
      (int)found.lexeme.length,
      found.lexeme.ptr,
      found.line,
      found.column
  );
}

Json_Value *json_parse_array(Parser_State *parser)
{
  if (!match(parser, TOKEN_TYPE_OPEN_BRACKET)) {
    parse_error(parser, "Expected '['");
    return NULL;
  }


  Json_Item_Array array = z_array_new(parser->heap, Json_Item_Array);

  do {
    z_array_push(&array, json_parse_value(parser));
  } while (match(parser, TOKEN_TYPE_COMMA));

  if (!match(parser, TOKEN_TYPE_CLOSE_BRACKET)) {
    parse_error(parser, "Expected ']'");
    return NULL;
  }

  return create_json_item_array(parser, array);
}

Json_Value *json_parse_value(Parser_State *parser)
{
  if (check(parser, TOKEN_TYPE_OPEN_BRACE)) {
    return json_parse_object(parser);
  }

  if (match(parser, TOKEN_TYPE_NUMBER)) {
    return create_json_item_number(parser, previous(parser).number_value);
  }

  if (match(parser, TOKEN_TYPE_STRING)) {
    return create_json_item_string(parser, previous(parser).lexeme);
  }

  if (check(parser, TOKEN_TYPE_OPEN_BRACKET)) {
    return json_parse_array(parser);
  }

  parse_error(parser, "Expected: object | string | number | array");
  return NULL;
}

void json_parse_key_value_pairs(Parser_State *parser, Z_Map *key_value_pairs)
{
  if (check(parser, TOKEN_TYPE_CLOSE_BRACE)) {
    return;
  }

  if (!check(parser, TOKEN_TYPE_STRING)) {
    parse_error(parser, "Expected key");
    return;
  }

  Token key = advance(parser);

  if (!match(parser, TOKEN_TYPE_COLON)) {
    parse_error(parser, "Expected ':'");
    return;
  }

  Json_Value *value = json_parse_value(parser);

  if (value == NULL) {
    return;
  }

  z_map_put(key_value_pairs, z_sv_to_cstr(parser->heap, key.lexeme), value);

  if (match(parser, TOKEN_TYPE_COMMA)) {
    json_parse_key_value_pairs(parser, key_value_pairs);
  }
}

Json_Value *json_parse_object(Parser_State *parser)
{
  if (!match(parser, TOKEN_TYPE_OPEN_BRACE)) {
    parse_error(parser, "Expected '{'");
    return NULL;
  }

  Z_Map key_value_pairs = z_map_new(parser->heap, (Z_Compare_Fn)strcmp);

  json_parse_key_value_pairs(parser, &key_value_pairs);

  if (!match(parser, TOKEN_TYPE_CLOSE_BRACE)) {
    parse_error(parser, "Expected '}'");
    return NULL;
  }

  return create_json_item_object(parser, key_value_pairs);
}

Json_Value *json_parse(Z_Heap *heap, Token_Array tokens)
{
  Parser_State parser = create_parser(heap, tokens);

  if (check(&parser, TOKEN_TYPE_OPEN_BRACE)) return json_parse_object(&parser);
  if (check(&parser, TOKEN_TYPE_OPEN_BRACKET)) return json_parse_array(&parser);
  return NULL;
}
