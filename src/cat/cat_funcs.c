#include "cat_funcs.h"

Flags CatReadFlags(int argc, char *argv[]) {
  struct option longOpts[] = {{"number-nonblank", 0, NULL, 'b'},
                              {"number", 0, NULL, 'n'},
                              {"squeeze-blank", 0, NULL, 's'},
                              {NULL, 0, NULL, 0}};

  int currentFlag = getopt_long(argc, argv, "bevEnstT", longOpts, NULL);
  Flags flags = {false, false, false, false, false, false};

  for (; currentFlag != -1;
       currentFlag = getopt_long(argc, argv, "bevEnstT", longOpts, NULL)) {
    switch (currentFlag) {
      case 'b':
        flags.flag_b = true;
        flags.numberAll = false;
        break;
      case 'e':
        flags.flag_e = true;
        flags.flag_v = true;
        break;
      case 'v':
        flags.flag_v = true;
        break;
      case 'E':
        flags.flag_e = true;
        break;
      case 'n':
        if (!flags.flag_b) flags.numberAll = true;
        break;
      case 's':
        flags.squeeze = true;
        break;
      case 't':
        flags.flag_v = true;
        flags.tab = true;
        break;
      case 'T':
        flags.tab = true;
        break;
    }
  }
  return flags;
}

void CatFile(FILE *file, Flags flags) {
  int c;
  c = getc(file);
  int last = '\n';
  int curLine = 1;
  bool squeeze = true;

  while (c != EOF) {
    if (!(flags.flag_b) && flags.numberAll && curLine == 1) {
      printf("%6d\t", curLine++);
    }
    if (flags.flag_b && curLine == 1 && c != '\n') {
      printf("%6d\t", curLine++);
    }
    if (flags.flag_e && c == '\n' && squeeze == true) {
      // if (flags.flag_b && last == '\n') {
      //   printf("%7c%c", '\t', '$');
      // } else
      printf("$");
    }
    if (flags.tab && c == '\t') {
      printf("^");
      c = 'I';
    }
    if (flags.flag_v) {
      if (c <= 31 && c != '\n' && c != '\t') {
        printf("^");
        c += 64;
      } else if (c == 127 && c != '\n' && c != '\t') {
        printf("^");
        c = '?';
      } else if (c >= 128 && c < 128 + 32 && c != '\n' && c != '\t') {
        printf("M-^");
        c -= 64;
      } else if (c >= 128 + 32 && c != '\n' && c != '\t') {
        printf("M-");
        c -= 128;
      }
    }  // -v
    if (squeeze) {
      printf("%c", c);
    }
    if (flags.squeeze && c == '\n' && last == '\n') {
      squeeze = false;
    } else if (c != '\n' && last == '\n' && squeeze == false) {
      printf("%c", c);
      squeeze = true;
    }

    last = c;
    c = getc(file);
    if (flags.flag_b && curLine > 1 && last == '\n' && c != '\n' && c != EOF) {
      printf("%6d\t", curLine++);
    }
    if (!(flags.flag_b) && flags.numberAll && curLine > 1 && last == '\n' &&
        c != EOF && (c != '\n' || squeeze == true)) {
      printf("%6d\t", curLine++);
    }
  }
}

void Cat(int argc, char *argv[], Flags flags) {
  for (char **filename = &argv[1], **end = &argv[argc]; filename != end;
       ++filename) {
    if (**filename == '-') continue;
    FILE *file = fopen(*filename, "r");
    if (file == NULL) {
      fprintf(stderr, "%s ", argv[0]);
      perror(*filename);
      continue;
    }
    CatFile(file, flags);
    fclose(file);
  }
}
