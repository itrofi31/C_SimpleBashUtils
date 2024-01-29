#define _GNU_SOURCE
#include "grep_funcs.h"

int GrepReadFlags(int argc, char *argv[], Flags *flags) {
  int res = 1;
  int currentFlag = getopt(argc, argv, "e:ivclnhsof:");

  for (; currentFlag != -1; currentFlag = getopt(argc, argv, "e:ivclnhsof:")) {
    switch (currentFlag) {
      case 'e':
        if (flags->pattern_count < MAX_PATTERNS) {
          flags->pattern[flags->pattern_count++] = optarg;
          flags->flag_e = true;
        } else {
          res = 0;
          printf("%s: Too many patterns\n", argv[0]);
        }
        break;
      case 'i':
        flags->flag_i = true;
        break;
      case 'v':
        flags->flag_v = true;
        break;
      case 'c':
        flags->flag_c = true;
        break;
      case 'l':
        flags->flag_l = true;
        break;
      case 'n':
        flags->flag_n = true;
        break;
      case 'h':
        flags->flag_h = true;
        break;
      case 's':
        flags->flag_s = true;
        break;
      case 'o':
        flags->flag_o = true;
        break;
      case 'f':
        flags->flag_f = true;
        flags->file = optarg;
        break;
      case '?':
        res = 0;
        break;
    }
  }
  if (flags->pattern_count == 0)
    flags->pattern[flags->pattern_count++] = argv[optind];

  return res;
}

void Grep(int argc, char *argv[], Flags *flags) {
  regex_t reg[flags->pattern_count];
  int count_files =
      flags->flag_e || flags->flag_f ? argc - optind : argc - optind - 1;
  int flag = flags->flag_i ? REG_ICASE : 0;
  char **filename =
      flags->flag_e || flags->flag_f ? argv + optind : argv + optind + 1;
  if (flags->pattern_count == 0) {
    fprintf(stderr, "no pattern specified\n");
    exit(1);
  }

  for (int i = 0; i < flags->pattern_count; i++) {
    if (regcomp(&reg[i], flags->pattern[i], flag)) {
      fprintf(stderr, "failed to compile regex\n");
      regfree(&reg[i]);
      exit(1);
    }
  }
  OpenFile(filename, count_files, flags, reg, argv);
  for (int i = 0; i < flags->pattern_count; i++) {
    regfree(&reg[i]);
  }
}

void OpenFile(char **filename, int count_files, Flags *flags, regex_t *reg,
              char *argv[]) {
  for (; *filename != NULL; ++filename) {
    FILE *file = fopen(*filename, "rb");
    if (file == NULL) {
      if (!flags->flag_s) {
        fprintf(stderr, "%s ", argv[0]);
        perror(*filename);
      }
      // fclose(file);
      continue;
    }
    if (flags->flag_c && !flags->flag_f && !flags->flag_v)
      GrepCount(file, *filename, count_files, flags, reg);
    else {
      flags->flag_f ? GrepFromFile(file, *filename, count_files, flags)
                    : GrepFile(file, *filename, count_files, flags, reg);
    }
    fclose(file);
  }
}

void GrepFile(FILE *file, const char *filename, int count_files, Flags *flags,
              regex_t *reg) {
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count_lines = 0;
  int count_reverse_lines = 0;
  while (getline(&line, &length, file) != -1) {
    count_lines++;
    bool matched = false;
    for (int i = 0; i < flags->pattern_count; i++) {
      if (regexec(&reg[i], line, 1, &match, 0) == 0) {
        matched = true;
        break;
      }
    }
    if (flags->flag_v && !matched && !flags->flag_l) {
      if (flags->flag_c) count_reverse_lines++;
      if (flags->flag_n)
        PrintCountlines(filename, line, count_lines, count_files);
      else if (!flags->flag_c)
        PrintLine(line, filename, count_files);
    }
    if (flags->flag_l && matched) {
      printf("%s\n", filename);
      break;
    }

    if (flags->flag_n && !flags->flag_o && matched && !flags->flag_e &&
        !flags->flag_v) {
      PrintCountlines(filename, line, count_lines, count_files);
    }
    if (flags->flag_e && matched && !flags->flag_v) {
      bool matc = false;
      for (int i = 0; i < flags->pattern_count; i++) {
        if (strcasestr(line, flags->pattern[i]) != NULL && !matc) {
          flags->flag_n ? printf("%d:", count_lines) : 0;
          flags->flag_o ? PrintMatchOnly(line, filename, count_lines,
                                         count_files, flags, match)
                        : PrintLine(line, filename, count_files);
          matc = true;
        }
      }
    }
    if (flags->flag_h && matched && !flags->flag_e && !flags->flag_v &&
        !flags->flag_n && !flags->flag_o) {
      line[strlen(line) - 1] == '\n' ? printf("%s", line)
                                     : printf("%s\n", line);
    }
    if (flags->flag_o && !flags->flag_c && matched && !flags->flag_e &&
        !flags->flag_v) {
      PrintMatchOnly(line, filename, count_lines, count_files, flags, match);
    } else {
      if (matched && !flags->flag_v && !flags->flag_e && !flags->flag_n &&
          !flags->flag_o && !flags->flag_c && !flags->flag_l && !flags->flag_h)
        PrintLine(line, filename, count_files);
    }
  };

  if (flags->flag_v && flags->flag_c) {
    printf("%d\n", count_reverse_lines);
  }
  free(line);
}

void GrepFromFile(FILE *file, char *filename, int count_files, Flags *flags) {
  char *line = 0;
  size_t length = 0;
  int match_count = 0;
  int count_lines = 0;

  (void)filename, (void)(count_files);
  FILE *pattern_file = fopen(flags->file, "rb");
  if (pattern_file == NULL) {
    fprintf(stderr, "%s ", flags->file);
    perror(flags->file);
    // regfree(reg);
    free(line);
    exit(1);
  }
  while (getline(&line, &length, pattern_file) != -1) {
    count_lines++;
  }
  fseek(pattern_file, 0, SEEK_SET);  // point file to the start
  regex_t reg[count_lines];
  int i = 0;
  while (getline(&line, &length, pattern_file) != -1) {
    line[strcspn(line, "\n")] =
        0;  // Remove newline character at the end of the line
    if (strlen(line) == 0) continue;
    if (regcomp(&reg[i], line, flags->flag_i ? REG_ICASE : 0)) {
      fprintf(stderr, "failed to compile regex\n");
      regfree(&reg[i]);
      free(line);
      fclose(pattern_file);
      exit(1);
    }
    i++;
  }
  fclose(pattern_file);

  fseek(file, 0, SEEK_SET);  // point file to the start
  int line_number = 0;
  while (getline(&line, &length, file) != -1) {
    line_number++;
    int matched = 0;
    for (int j = 0; j < count_lines; j++) {
      if (regexec(&reg[j], line, 0, NULL, 0) == 0) {
        matched = 1;
        break;
      }
    }
    if ((matched && !flags->flag_v)) {
      if (flags->flag_c)
        match_count++;
      else {
        flags->flag_n
            ? PrintCountlines(filename, line, line_number, count_files)
            : printf("%s", line);
      }
    } else if (!matched && flags->flag_v) {
      if (flags->flag_c)
        match_count++;
      else {
        flags->flag_n
            ? PrintCountlines(filename, line, line_number, count_files)
            : printf("%s", line);
      }
    }
  }

  for (int j = 0; j < count_lines; j++) {
    regfree(&reg[j]);
  }
  free(line);

  flags->flag_c ? printf("%d\n", match_count) : 0;
}

int GrepCount(FILE *file, const char *filename, int count_files,
              const Flags *flags, regex_t *reg) {
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count_lines = 0;
  while (getline(&line, &length, file) != -1) {
    if (flags->flag_c) {
      for (int i = 0; i < flags->pattern_count; i++) {
        if (regexec(&reg[i], line, 1, &match, 0) == 0) count_lines++;
      }
    }
  }
  if (!flags->flag_f) {
    (count_files == 1) ? printf("%d\n", count_lines)
                       : printf("%s:%d\n", filename, count_lines);
  }
  free(line);
  if (flags->flag_l) printf("%s\n", filename);
  return count_lines;
}

void PrintLine(const char *line, const char *filename, int count_files) {
  if (line[strlen(line) - 1] != '\n') {
    count_files == 1 ? printf("%s\n", line) : printf("%s:%s\n", filename, line);
  } else {
    count_files == 1 ? printf("%s", line) : printf("%s:%s", filename, line);
  }
}

void PrintCountlines(const char *filename, const char *line, int count_lines,
                     int count_files) {
  if (line[strlen(line) - 1] != '\n')
    count_files == 1 ? printf("%d:%s\n", count_lines, line)
                     : printf("%s:%d:%s\n", filename, count_lines, line);
  else
    count_files == 1 ? printf("%d:%s", count_lines, line)
                     : printf("%s:%d:%s", filename, count_lines, line);
}

void PrintMatchOnly(char *line, const char *filename, int count_lines,
                    int count_files, const Flags *flags, regmatch_t match) {
  const char *start = line + match.rm_so;
  char *end = line + match.rm_eo;
  char saved = *end;
  *end = '\0';
  if (flags->flag_n)
    count_files == 1 ? printf("%d:%s\n", count_lines, start)
                     : printf("%s:%d:%s\n", filename, count_lines, start);
  else
    count_files == 1 ? printf("%s\n", start)
                     : printf("%s:%s\n", filename, start);
  *end = saved;
}