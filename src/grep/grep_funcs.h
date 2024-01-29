#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PATTERNS 100

typedef struct {
  int flag_i;
  bool flag_e, flag_v, flag_c, flag_l, flag_n, flag_h, flag_s, flag_o, flag_f;
  char *file, *pattern[MAX_PATTERNS];
  int pattern_count;
} Flags;

int GrepReadFlags(int argc, char *argv[], Flags *flags);
void Grep(int argc, char *argv[], Flags *flags);
void OpenFile(char **filename, int count_files, Flags *flags, regex_t *reg,
              char *argv[]);
void GrepFile(FILE *file, const char *filename, int count_files, Flags *flags,
              regex_t *reg);
void GrepFromFile(FILE *file, char *filename, int count_files, Flags *flags);
int GrepCount(FILE *file, const char *filename, int count_files,
              const Flags *flags, regex_t *reg);
void PrintLine(const char *line, const char *filename, int count_files);
void PrintCountlines(const char *filename, const char *line, int count_lines,
                     int count_files);
void PrintMatchOnly(char *line, const char *filename, int count_lines,
                    int count_files, const Flags *flags, regmatch_t match);