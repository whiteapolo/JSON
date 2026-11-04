#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../zlib/include/string.h"
#include "../zlib/include/file.h"
#include "../zlib/include/error.h"
#include "../zlib/include/time.h"

#include "json.c"
#include "json-stringify.c"
#include "json.h"

int main(void)
{
    Z_Pool_Auto pool = {0};
    Z_Time start = z_time();
    Json_Parse_Result result = json_parse_file(&pool, "example-data/5MB.json");
    z_time_print_elapsed_mseconds(start);

    if (result.had_error) {
        fprintf(stderr, "%s\n", result.error);
    }

    // Z_String s = z_str_new(&pool, "");
    // json_stringify(&result.json, &s);
    // z_sv_print(z_sv(s));

    return 0;
}
