#include "cat_funcs.h"

int main(int argc, char *argv[]) {
  Flags flags = CatReadFlags(argc, argv);
  Cat(argc, argv, flags);

  return 0;
}
