#include "json.h"
#include "../zlib/include/scanner.h"
#include "../zlib/include/time.h"
#include <stdalign.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

Json_Value JSON_DUMMY = {0};

typedef struct {
    Z_Pool *pool;
    bool had_error;
    Z_Scanner scanner;
    char *error;
} Parser_State;

Json_Value json_parse_value(Parser_State *parser);

Json_Value json_create_boolean(bool boolean)
{
    Json_Value value = {
        .type = JSON_BOOLEAN,
        .boolean = boolean,
    };

    return value;
}

Json_Value json_create_null()
{
    Json_Value value = {
        .type = JSON_NULL,
    };

    return value;
}

Json_Value json_create_string(Z_String string)
{
    Json_Value value = {
        .type = JSON_STRING,
        .string = string,
    };

    return value;
}

Json_Value json_create_number(double number)
{
    Json_Value value = {
        .type = JSON_NUMBER,
        .number = number,
    };

    return value;
}

Json_Value json_create_array(Json_Value_Array array)
{
    Json_Value value = {
        .type = JSON_ARRAY,
        .array = array,
    };

    return value;
}

Json_Value json_create_object(Z_Hash_Table object)
{
    Json_Value value = {
        .type = JSON_OBJECT,
        .object = object,
    };

    return value;
}

Json_Value json_error(Parser_State *parser, const char *message)
{
    parser->had_error = true;

    Z_String error = z_str_new(
        parser->pool,
        "%zu:%zu: %s",
        parser->scanner.line,
        parser->scanner.column,
        message
    );

    parser->error = error.ptr;
    return JSON_DUMMY;
}

Json_Value json_parse_true(Parser_State *parser)
{
    if (z_scanner_match_string(&parser->scanner, z_sv("true"))) {
        return json_create_boolean(true);
    }

    return json_error(parser, "Expected true");
}

Json_Value json_parse_false(Parser_State *parser)
{
    if (z_scanner_match_string(&parser->scanner, z_sv("false"))) {
        return json_create_boolean(false);
    }

    return json_error(parser, "Expected false");
}

Json_Value json_parse_null(Parser_State *parser)
{
    if (z_scanner_match_string(&parser->scanner, z_sv("null"))) {
        return json_create_null();
    }

    return json_error(parser, "Expected null");
}

Json_Value json_parse_string(Parser_State *parser)
{
    if (!z_scanner_match(&parser->scanner, '"')) {
        return json_error(parser, "Expected string");
    }

    Z_String string = z_str_new(parser->pool, "");

    while (
        !z_scanner_is_at_end(&parser->scanner)
        && z_scanner_peek(&parser->scanner) != '"'
        && z_scanner_peek(&parser->scanner) != '\n'
    ) {
        if (z_scanner_match(&parser->scanner, '\\')) {
            if (z_scanner_is_at_end(&parser->scanner)) {
                return json_error(parser, "Expected end of string, got end of file");
            } else {
                z_str_push_back_char(&string, z_scanner_advance(&parser->scanner));
            }
        } else {
            z_str_push_back_char(&string, z_scanner_advance(&parser->scanner));
        }
    }

    if (z_scanner_match(&parser->scanner, '\n')) {
        return json_error(parser, "Expected end of string, got new line");
    }

    if (!z_scanner_match(&parser->scanner, '"')) {
        return json_error(parser, "Expected end of string, got end of file");
    }

    return json_create_string(string);
}

bool parse_integer(Parser_State *parser, int *number)
{
    if (!isdigit(z_scanner_peek(&parser->scanner))) {
        return false;
    }

    *number = 0;

    while (!z_scanner_is_at_end(&parser->scanner) && isdigit(z_scanner_peek(&parser->scanner))) {
        *number *= 10;
        *number += z_scanner_advance(&parser->scanner) - '0';
    }

    return true;
}

double build_number_from_parts(int sign, int number, int fraction,
                               int exponent_sign, int exponent)
{
    return ((sign * (double)number) + (fraction / pow(10, log10(fraction) + 1))) * pow(10, exponent_sign * exponent);
}

Json_Value json_parse_number(Parser_State *parser)
{
    int sign = 1;
    int number = 0;
    int fraction = 0;
    int exponent = 1;
    int exponent_sign = 1;

    if (z_scanner_match(&parser->scanner, '-')) {
        sign = -1;
    }

    if (!parse_integer(parser, &number)) {
        return json_error(parser, "Expected number");
    }

    if (z_scanner_match(&parser->scanner, '.')) {
        if (!parse_integer(parser, &fraction)) {
            return json_error(parser, "Expected number");
        }
    }

    if (z_scanner_match(&parser->scanner, 'e') || z_scanner_match(&parser->scanner, 'E')) {
        if (z_scanner_match(&parser->scanner, '-')) {
            exponent_sign = -1;
        } else {
            z_scanner_match(&parser->scanner, '+');
        }

        if (!parse_integer(parser, &exponent)) {
            return json_error(parser, "Expected number");
        }
    }

    return json_create_number(build_number_from_parts(sign, number, fraction, exponent_sign, exponent));
}

Json_Value json_parse_array(Parser_State *parser)
{
    if (parser->had_error) {
        return JSON_DUMMY;
    }

    z_scanner_skip_spaces(&parser->scanner);

    if (!z_scanner_match(&parser->scanner, '[')) {
        return json_error(parser, "Expected '['");
    }

    Json_Value_Array array = z_array_new(parser->pool, Json_Value_Array);

    if (z_scanner_match(&parser->scanner, ']')) {
        return json_create_array(array);
    }

    do {
        z_scanner_skip_spaces(&parser->scanner);
        Json_Value value = json_parse_value(parser);

        if (parser->had_error) {
            return JSON_DUMMY;
        }

        z_array_push_back(&array, value);
        z_scanner_skip_spaces(&parser->scanner);
    } while (z_scanner_match(&parser->scanner, ','));

    Json_Value value = json_parse_value(parser);

    if (parser->had_error) {
        return JSON_DUMMY;
    }

    z_array_push_back(&array, value);

    if (!z_scanner_match(&parser->scanner, ']')) {
        return json_error(parser, "Expected ']'");
    }

    return json_create_array(array);
}

Json_Value json_parse_object(Parser_State *parser)
{
    if (parser->had_error) {
        return JSON_DUMMY;
    }

    z_scanner_skip_spaces(&parser->scanner);

    if (!z_scanner_match(&parser->scanner, '{')) {
        return json_error(parser, "Expected '{'");
    }

    Z_Hash_Table object = z_hash_table_new(parser->pool, z_str_equal, z_str_hash);

    if (z_scanner_match(&parser->scanner, '}')) {
        return json_create_object(object);
    }

    do {
        z_scanner_skip_spaces(&parser->scanner);
        Json_Value key = json_parse_string(parser);

        if (parser->had_error) {
            return JSON_DUMMY;
        }

        z_scanner_skip_spaces(&parser->scanner);

        if (!z_scanner_match(&parser->scanner, ':')) {
            return json_error(parser, "Expected ':'");
        }

        z_scanner_skip_spaces(&parser->scanner);

        Json_Value value = json_parse_value(parser);

        if (parser->had_error) {
            return JSON_DUMMY;
        }

        z_hash_table_put(&object, key.string.ptr, z_pool_memory_duplicate(parser->pool, &value, sizeof(Json_Value)));
        z_scanner_skip_spaces(&parser->scanner);
    } while (z_scanner_match(&parser->scanner, ','));

    Json_Value key = json_parse_string(parser);

    if (parser->had_error) {
        return JSON_DUMMY;
    }

    z_scanner_skip_spaces(&parser->scanner);

    if (!z_scanner_match(&parser->scanner, ':')) {
        return json_error(parser, "Expected ':'");
    }

    z_scanner_skip_spaces(&parser->scanner);

    Json_Value value = json_parse_value(parser);

    if (parser->had_error) {
        return JSON_DUMMY;
    }

    z_hash_table_put(&object, key.string.ptr, z_pool_memory_duplicate(parser->pool, &value, sizeof(Json_Value)));

    if (!z_scanner_match(&parser->scanner, '}')) {
        return json_error(parser, "Expected '}'");
    }

    return json_create_object(object);
}

Json_Value json_parse_value(Parser_State *parser)
{
    z_scanner_skip_spaces(&parser->scanner);

    if (parser->had_error) {
        return JSON_DUMMY;
    }

    if (z_scanner_is_at_end(&parser->scanner)) {
        return json_error(parser, "Expected expression, got end of file");
    }

    z_scanner_reset_mark(&parser->scanner);

    switch (z_scanner_peek(&parser->scanner)) {
        case '"': return json_parse_string(parser);
        case '[': return json_parse_array(parser);
        case '{': return json_parse_object(parser);

        case 't': return json_parse_true(parser);
        case 'f': return json_parse_false(parser);
        case 'n': return json_parse_null(parser);

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.': case 'e': case '-': case '+':
            return json_parse_number(parser);

        default: return json_error(parser, "Expected: string | number | boolean | null | array | object");
    }
}

Json_Parse_Result json_parse(Z_Pool *pool, Z_String_View document)
{
    Parser_State parser = {
        .pool = pool,
        .had_error = false,
        .scanner = z_scanner_new(document),
    };

    Json_Value json = json_parse_value(&parser);

    Json_Parse_Result result = {
        .had_error = parser.had_error,
        .error = parser.error,
        .json = json,
    };

    return result;
}

Json_Parse_Result json_parse_file(Z_Pool *pool, const char *pathname)
{
    Z_Pool_Auto scratch = {0};
    Z_String document = z_str_new(&scratch, "");

    if (!z_str_push_back_file(&document, pathname)) {

        Json_Parse_Result result = {
            .had_error = true,
            .error = z_str_new(pool, "Cannot read file: '%s': %s", pathname, strerror(errno)).ptr,
        };

        return result;
    }

    return json_parse(pool, z_sv(document));
}
