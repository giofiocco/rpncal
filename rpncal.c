#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "module.h"

typedef struct {
  char *start;
  int len;
} sv_t;

#define SV_FMT            "%*.*s"
#define SV_UNPACK(__sv__) (__sv__.len), (__sv__.len), (__sv__.start)

sv_t sv_from_cstr(char *s) {
  return (sv_t){s, strlen(s)};
}

bool sv_eq(sv_t a, sv_t b) {
  if (a.len != b.len) {
    return false;
  }
  return memcmp(a.start, b.start, a.len) == 0;
}

typedef enum {
  T_NONE,
  T_OP,
  T_NUM,
} token_kind_t;

typedef struct {
  token_kind_t kind;
  union {
    double num;
    sv_t sv;
  } as;
} token_t;

token_t token_next(char **_buffer) {
  char *buffer = *_buffer;

  char *endnum = NULL;
  double num = strtod(buffer, &endnum);

  if (*buffer == 0) {
    return (token_t){T_NONE, {}};
  } else if (isspace(*buffer)) {
    (*_buffer)++;
    return token_next(_buffer);
  } else if (buffer[0] == '.' && (isspace(buffer[1]) || buffer[1] == 0)) {
    (*_buffer)++;
    return (token_t){T_OP, {.sv = (sv_t){buffer, 1}}};
  } else if (num == 0 && endnum == buffer) {
    int i = 0;
    while (!isspace(buffer[i]) && buffer[i] != 0) {
      i++;
    }
    *_buffer += i;
    return (token_t){T_OP, {.sv = (sv_t){buffer, i}}};
  } else {
    *_buffer += endnum - buffer;
    return (token_t){T_NUM, {.num = num}};
  }
}

int load_module(calc_t *calc, char *module_path) {
  assert(calc);

  assert(calc->module_count + 1 < MAX_MODULES);
  void *lib = dlopen(module_path, RTLD_NOW);
  if (!lib) {
    eprint("module not loaded: '%s': %s", module_path, dlerror());
    return 0;
  }
  calc->modules[calc->module_count++] = lib;

  ((void (*)(calc_t *))dlsym(lib, "load"))(calc);

  return 1;
}

void default_printer(double *stack, int count) {
  for (int i = 0; i < count; ++i) {
    mvprintw(count - i, 0, "%d: %s%E    %f\n", count - i - 1, stack[i] < 0 ? "" : " ", stack[i], stack[i]);
  }
}

void print_help() {
  printf("Usage: rpncal [options]\n\n"
         "Options:\n"
         "  -c             enable colors if possible\n"
         "  -h | --help    print help and exit\n");
}

int main(int argc, char **argv) {
  if (!isatty(1)) {
    fprintf(stderr, "ERROR: unable to run in this terminal\n");
    return 1;
  }

  bool colors = false;

  for (int i = 1; i < argc; ++i) {
    if (strcmp("-c", argv[i]) == 0) {
      colors = true;
    } else if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
      print_help();
      return 1;
    }
  }

  calc_t calc = {0};
  calc.printers[calc.printer_count++] = (printer_t){"default", default_printer};

  double stack[2048] = {0};
  int head = 0;
  char buffer[80] = {0};

  initscr();
  noecho();
  keypad(stdscr, true);
  if (colors && has_colors()) {
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
  }

  char *autoload_modules[] = {
      "./modules/core.so",
      "./modules/trig.so",
  };

  int autoload_count = sizeof(autoload_modules) / sizeof(autoload_modules[0]);
  for (int i = 0; i < autoload_count; ++i) {
    load_module(&calc, autoload_modules[i]);
  }

  char *helps[][2] = {
      {"include <name>", "include module"},
      {"quit | exit", "end program"},
      {"? | help", "print all operations"},
      {".", "drop first element"},
      {":", "duplicate first element"},
  };

  while (1) {
    erase();
    mvprintw(0, 0, "> ");
    calc.printers[calc.current_printer].func(stack, head);
    refresh();
    move(0, 2);

    memset(buffer, 0, 80);
    int len = 0;
    int i = 0;
    int c = 0;
    while ((c = getch()) != '\n' && c != 0 && i < 80) {
      if (c == KEY_LEFT) {
        if (i > 0) {
          i--;
        }
      } else if (c == KEY_RIGHT) {
        if (i < len) {
          i++;
        }
      } else if (c == KEY_BACKSPACE) {
        if (i > 0) {
          i--;
          memcpy(&buffer[i], &buffer[i + 1], 80 - i);
          len--;
        }

      } else if (isprint(c)) {
        memcpy(&buffer[i + 1], &buffer[i], 80 - i);
        buffer[i++] = c;
        len = i > len ? i : len;
      }
      move(0, 0);
      clrtoeol();
      printw("> %s", buffer);
      move(0, 2 + i);
    }

    if (*buffer == 0) {
      continue;
    }

    char *_buffer = buffer;

    token_t token = {0};
    while ((token = token_next(&_buffer)).kind != T_NONE) {

      if (token.kind == T_OP && sv_eq(token.as.sv, sv_from_cstr("include"))) {
        token = token_next(&_buffer);
        if (token.kind != T_OP) {
          eprint_("include expects an argument");
        }

        char path[90] = {0};
        if (*token.as.sv.start == '/') {
          snprintf(path, 90, SV_FMT ".so", SV_UNPACK(token.as.sv));
        } else {
          snprintf(path, 90, "./" SV_FMT ".so", SV_UNPACK(token.as.sv));
        }

        int start_ops = calc.op_count;
        int start_printers = calc.printer_count;
        if (load_module(&calc, path)) {
          mvprintw(0, 0, "loaded %d operations and %d printers from module %s", calc.op_count - start_ops, calc.printer_count - start_printers, path);
          getch();
        }

      } else if (token.kind == T_OP && (sv_eq(token.as.sv, sv_from_cstr("exit")) || sv_eq(token.as.sv, sv_from_cstr("quit")))) {
        goto close;

      } else if (token.kind == T_OP && (sv_eq(token.as.sv, sv_from_cstr("?")) || sv_eq(token.as.sv, sv_from_cstr("help")))) {
        erase();

        int maxw = 0;
        int count = sizeof(helps) / sizeof(helps[0]);
        for (int i = 0; i < count; ++i) {
          mvprintw(i, 0, "%s", helps[i][0]);
          int w = strlen(helps[i][0]);
          maxw = w > maxw ? w : maxw;
        }

        for (int i = 0; i < calc.op_count; ++i) {
          move(count + i, 0);
          int w = 0;
          for (int j = 0; j < MAX_KW_COUNT && calc.operations[i].kws[j]; ++j) {
            if (j > 0) {
              printw(" | ");
              w += 3;
            }
            printw("%s", calc.operations[i].kws[j]);
            w += strlen(calc.operations[i].kws[j]);
          }
          maxw = w > maxw ? w : maxw;
        }

        for (int i = 0; i < count; ++i) {
          mvprintw(i, maxw + 4, "%s", helps[i][1]);
        }

        for (int i = 0; i < calc.op_count; ++i) {
          mvprintw(count + i, maxw + 4, "%s", calc.operations[i].description);
        }

        move(count + calc.op_count, 0);

        printw("Printers:\n");
        for (int i = 0; i < calc.printer_count; ++i) {
          printw("%s\n", calc.printers[i].name);
        }

        refresh();
        getch();
      } else if (token.kind == T_OP && sv_eq(token.as.sv, sv_from_cstr("."))) {
        if (head > 0) {
          head--;
        }
      } else if (token.kind == T_OP && sv_eq(token.as.sv, sv_from_cstr(":"))) {
        if (head > 0) {
          stack[head] = stack[head - 1];
          head++;
        }
      } else if (token.kind == T_OP) {

        bool is_found = false;
        for (int i = 0; i < calc.op_count; ++i) {
          for (int j = 0; j < MAX_KW_COUNT && calc.operations[i].kws[j]; ++j) {
            if (sv_eq(sv_from_cstr(calc.operations[i].kws[j]), token.as.sv)) {
              is_found = true;
              if (head < calc.operations[i].nargs) {
                eprint("'%s' needs at least %d args", buffer, calc.operations[i].nargs);
                goto found;
              }
              calc.operations[i].func(stack, &head);
              goto found;
            }
          }
        }
        for (int i = 0; i < calc.printer_count; ++i) {
          if (sv_eq(sv_from_cstr(calc.printers[i].name), token.as.sv)) {
            is_found = true;
            calc.current_printer = i;
            goto found;
          }
        }
      found:
        if (!is_found) {
          eprint("operation not found: '" SV_FMT "'", SV_UNPACK(token.as.sv));
          continue;
        }
      } else if (token.kind == T_NUM) {
        stack[head++] = token.as.num;
      } else {
        assert(0 && "unreachable");
      }
    }

    /*
    if (strcmp(_buffer, "include") == 0) {
      char *argstart = strchr(buffer, ' ') + 1;
      char arg[90] = {0};
      if (*argstart == '/') {
        snprintf(arg, 90, "%s.so", argstart);
      } else {
        snprintf(arg, 90, "./%s.so", argstart);
      }

      int start_ops = calc.op_count;
      int start_printers = calc.printer_count;
      if (load_module(&calc, arg)) {
        mvprintw(0, 0, "loaded %d operations and %d printers from module %s", calc.op_count - start_ops, calc.printer_count - start_printers, arg);
        getch();
      }

    } else if (strcmp(buffer, "quit") == 0 || strcmp(buffer, "exit") == 0) {
      break;

    } else if (strcmp(buffer, "?") == 0 || strcmp(buffer, "help") == 0) {
      erase();

      int maxw = 0;
      int count = sizeof(helps) / sizeof(helps[0]);
      for (int i = 0; i < count; ++i) {
        mvprintw(i, 0, "%s", helps[i][0]);
        int w = strlen(helps[i][0]);
        maxw = w > maxw ? w : maxw;
      }

      for (int i = 0; i < calc.op_count; ++i) {
        move(count + i, 0);
        int w = 0;
        for (int j = 0; j < MAX_KW_COUNT && calc.operations[i].kws[j]; ++j) {
          if (j > 0) {
            printw(" | ");
            w += 3;
          }
          printw("%s", calc.operations[i].kws[j]);
          w += strlen(calc.operations[i].kws[j]);
        }
        maxw = w > maxw ? w : maxw;
      }

      for (int i = 0; i < count; ++i) {
        mvprintw(i, maxw + 4, "%s", helps[i][1]);
      }

      for (int i = 0; i < calc.op_count; ++i) {
        mvprintw(count + i, maxw + 4, "%s", calc.operations[i].description);
      }

      move(count + calc.op_count, 0);

      printw("Printers:\n");
      for (int i = 0; i < calc.printer_count; ++i) {
        printw("%s\n", calc.printers[i].name);
      }

      refresh();
      getch();

    } else if (strcmp(buffer, ".") == 0) {
      if (head < 1) {
        eprint("'%s' needs 1 arg", buffer);
        continue;
      }
      head--;

    } else if (strcmp(buffer, ":") == 0) {
      if (head < 1) {
        eprint("'%s' needs 1 arg", buffer);
        continue;
      }
      stack[head] = stack[head - 1];
      head++;

    } else if (('0' <= buffer[0] && buffer[0] <= '9') || ((buffer[0] == '.' || buffer[0] == '-') && buffer[1] != '\0')) {
      stack[head++] = strtod(buffer, NULL);

    } else {
      bool is_found = false;
      for (int i = 0; i < calc.op_count; ++i) {
        for (int j = 0; j < MAX_KW_COUNT && calc.operations[i].kws[j]; ++j) {
          if (strcmp(calc.operations[i].kws[j], buffer) == 0) {
            is_found = true;
            if (head < calc.operations[i].nargs) {
              eprint("'%s' needs at least %d args", buffer, calc.operations[i].nargs);
              goto found;
            }
            calc.operations[i].func(stack, &head);
            goto found;
          }
        }
      }
      for (int i = 0; i < calc.printer_count; ++i) {
        if (strcmp(calc.printers[i].name, buffer) == 0) {
          is_found = true;
          calc.current_printer = i;
          goto found;
        }
      }
    found:
      if (!is_found) {
        eprint("operation not found: '%s'", buffer);
        continue;
      }
    }
  */
  }
close:

  endwin();

  for (int i = 0; i < calc.module_count; ++i) {
    dlclose(calc.modules[i]);
  }

  return 0;
}
