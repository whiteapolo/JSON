#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "print-json.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../zlib/include/string.h"
#include "../zlib/include/file.h"
#include "../zlib/include/error.h"

#include "lexer.c"
#include "parser.c"
#include "token.c"
#include "print-json.c"

int main(void)
{
    Z_Pool_Auto pool = {0};
    Z_String content = z_str_new(&pool, "");

    if (!z_str_push_back_file(&content, "./example-data/data.json")) {
        z_perror("Cannot read json file\n");
        return 1;
    }

    Json_Lexer_Result lexer_result = json_lex(&pool, z_sv(content));

    if (!lexer_result.ok) {
        return 2;
    }

    // json_print_tokens(lexer_result.tokens);

    Json_Value *json = json_parse(&pool, lexer_result.tokens);

    if (json) {
        print_json(json);
    }

    return 0;
}
