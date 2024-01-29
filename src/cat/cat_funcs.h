#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  bool flag_b;
  bool flag_e;
  bool numberAll;
  bool squeeze;
  bool tab;
  bool flag_v;
} Flags;

Flags CatReadFlags(int argc, char *argv[]);
void CatFile(FILE *file, Flags flags);
void Cat(int argc, char *argv[], Flags flags);
