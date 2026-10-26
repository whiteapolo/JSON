#include "parser.h"
#include "token.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "../zlib/include/scanner.h"

typedef struct {
    Z_Pool *pool;
    Z_Scanner scanner;
} Parser_State;

bool is_number_character(char c);
bool string_to_number(Z_String_View s, double *result);

Json_Value *json_parse(Z_Pool *pool, Z_String_View document);
Json_Value *json_parse_value(Parser_State *state);
Json_Value *json_parse_object(Parser_State *state);
Json_Value *json_parse_array(Parser_State *state);
Json_Value *json_parse_number(Parser_State *state);
Json_Value *json_parse_string(Parser_State *state);

Json_Value *create_json_object(Z_Pool *pool, Z_Hash_Table object);
Json_Value *create_json_array(Z_Pool *pool, Json_Value_Array array);
Json_Value *create_json_null(Z_Pool *pool);
Json_Value *create_json_string(Z_Pool *pool, Z_String_View value);
Json_Value *create_json_boolean(Z_Pool *pool, bool value);
Json_Value *create_json_number(Z_Pool *pool, double value);

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

Json_Value *create_json_string(Z_Pool *pool, Z_String_View value)
{
    Json_Value *result = z_pool_malloc(pool, sizeof(Json_Value));
    result->kind = JSON_VALUE_STRING;
    result->string = z_str_new_from_sv(pool, value);

    return result;
}

Json_Value *create_json_boolean(Z_Pool *pool, bool value)
{
    Json_Value *result = z_pool_malloc(pool, sizeof(Json_Value));
    result->kind = JSON_VALUE_BOOLEAN;
    result->boolean = value;

    return result;
}

Json_Value *create_json_number(Z_Pool *pool, double value)
{
    Json_Value *result = z_pool_malloc(pool, sizeof(Json_Value));
    result->kind = JSON_VALUE_NUMBER;
    result->number = value;

    return result;
}

Json_Value *create_json_null(Z_Pool *pool)
{
    Json_Value *result = z_pool_malloc(pool, sizeof(Json_Value));
    result->kind = JSON_VALUE_NULL;

    return result;
}

Json_Value *create_json_array(Z_Pool *pool, Json_Value_Array array)
{
    Json_Value *result = z_pool_malloc(pool, sizeof(Json_Value));
    result->kind = JSON_VALUE_ARRAY;
    result->array = array;

    return result;
}

Json_Value *create_json_object(Z_Pool *pool, Z_Hash_Table object)
{
    Json_Value *result = z_pool_malloc(pool, sizeof(Json_Value));
    result->kind = JSON_VALUE_OBJECT;
    result->object = object;

    return result;
}

bool parse_string_literal(Parser_State *state, Z_String_View *result)
{
    z_scanner_reset_mark(&state->scanner);
    z_scanner_advance_until(&state->scanner, '"');

    if (z_scanner_is_at_end(&state->scanner)) {
        return false;
    }

    *result = z_scanner_capture(&state->scanner);
    z_scanner_advance(&state->scanner);

    return true;
}

Json_Value *json_parse_string(Parser_State *state)
{
    z_scanner_reset_mark(&state->scanner);
    z_scanner_advance_until(&state->scanner, '"');

    if (z_scanner_is_at_end(&state->scanner)) {
        return NULL;
    }

    Z_String_View value = z_scanner_capture(&state->scanner);
    Json_Value *result = create_json_string(state->pool, value);
    z_scanner_advance(&state->scanner);

    return result;
}

Json_Value *json_parse_number(Parser_State *state)
{
    while (!z_scanner_is_at_end(&state->scanner) && is_number_character(z_scanner_peek(&state->scanner))) {
        z_scanner_advance(&state->scanner);
    }

    Z_String_View lexeme = z_scanner_capture(&state->scanner);
    double value;

    if (!string_to_number(lexeme, &value)) {
        return NULL;
    }

    return create_json_number(state->pool, value);
}

Json_Value *json_parse_array(Parser_State *state)
{
    z_scanner_skip_spaces(&state->scanner);
    Json_Value_Array array = z_array_new(state->pool, Json_Value_Array);

    if (z_scanner_match(&state->scanner, ']')) {
        return create_json_array(state->pool, array);
    }

    do {
        Json_Value *item = json_parse_value(state);
        if (item == NULL) return NULL;
        z_array_push_back(&array, *item);
        z_scanner_skip_spaces(&state->scanner);
    } while (z_scanner_match(&state->scanner, ','));

    if (!z_scanner_match(&state->scanner, ']')) {
        return NULL;
    }

    return create_json_array(state->pool, array);
}

Json_Value *json_parse_object(Parser_State *state)
{
    z_scanner_skip_spaces(&state->scanner);

    Z_Hash_Table object = z_hash_table_new(state->pool, z_str_equal, z_str_hash);

    if (z_scanner_match(&state->scanner, '}')) {
        return create_json_object(state->pool, object);
    }

    do {
        z_scanner_skip_spaces(&state->scanner);
        if (!z_scanner_match(&state->scanner, '"')) return NULL;
        Z_String_View key;
        if (!parse_string_literal(state, &key)) return NULL;
        z_scanner_skip_spaces(&state->scanner);
        if (!z_scanner_match(&state->scanner, ':')) return NULL;
        Json_Value *value = json_parse_value(state);
        if (value == NULL) return NULL;

        z_hash_table_put(&object, z_sv_to_cstr(state->pool, key), value);
        z_scanner_skip_spaces(&state->scanner);
    } while (z_scanner_match(&state->scanner, ','));

    if (!z_scanner_match(&state->scanner, '}')) {
        return NULL;
    }

    return create_json_object(state->pool, object);
}

Json_Value *json_parse_value(Parser_State *state)
{
    z_scanner_skip_spaces(&state->scanner);
    z_scanner_reset_mark(&state->scanner);

    if (z_scanner_match_string(&state->scanner, z_sv("null"))) {
        return create_json_null(state->pool);
    }

    if (z_scanner_match_string(&state->scanner, z_sv("true"))) {
        return create_json_boolean(state->pool, true);
    }

    if (z_scanner_match_string(&state->scanner, z_sv("false"))) {
        return create_json_boolean(state->pool, false);
    }

    char c = z_scanner_advance(&state->scanner);

    if (is_number_character(c)) {
        return json_parse_number(state);
    }

    switch (c) {
        case '"': return json_parse_string(state);
        case '[': return json_parse_array(state);
        case '{': return json_parse_object(state);
        default:  return NULL;
    }
}

Json_Value *json_parse(Z_Pool *pool, Z_String_View document)
{
    Parser_State parser = {
        .pool = pool,
        .scanner = z_scanner_new(document),
    };

    return json_parse_value(&parser);
}
