#include <sys/wait.h>
#define LIBZATAR_IMPLEMENTATION
#include "libzatar.h"

int main(int argc, char **argv) {
  z_rebuild_yourself(__FILE__, argv);

  Z_Cmd cmd = {0};
  z_cmd_append(&cmd, "cc");
  z_cmd_append(&cmd, "main.c", "lexer.c", "token.c", "parser.c", "print_json.c");
  z_cmd_append(&cmd, "-o", "exe");
  z_cmd_append(&cmd, "-Wextra", "-Wall", "-pedantic");
  z_cmd_append(&cmd, "-g", "-lm");
  z_cmd_append(&cmd, "-O0");

  z_cmd_run_sync(&cmd);

  return 0;
}
