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
#include "../zlib/include/time.h"

#include "parser.c"
#include "print-json.c"

int main(void)
{
    Z_Pool_Auto pool = {0};
    Z_String content = z_str_new(&pool, "");
    Z_Time start = z_time();

    if (!z_str_push_back_file(&content, "./example-data/5MB.json")) {
        z_perror("Cannot read json file");
        return 1;
    }

    Json_Value *json = json_parse(&pool, z_sv(content));
    // print_json(json);

    printf("%p\n", json);

    z_time_print_elapsed_mseconds(start);

    return 0;
}
