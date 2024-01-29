#include "grep_funcs.h"

int main(int argc, char *argv[]) {
  if (argc > 2) {
    Flags flags = {0,     false, false, false, false, false, false,
                   false, false, false, NULL,  {0},   0};
    if (GrepReadFlags(argc, argv, &flags)) Grep(argc, argv, &flags);
  }
  return 0;
}
