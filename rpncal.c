#include <assert.h>
#include <dlfcn.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "module.h"

// TODO: div doesnt work

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

int main() {
  if (!isatty(1)) {
    fprintf(stderr, "ERROR: unable to run in this terminal\n");
    return 1;
  }

  calc_t calc = {0};
  calc.printers[calc.printer_count++] = (printer_t){"default printer", default_printer};

  double stack[2048] = {0};
  int head = 0;
  char buffer[80] = {0};

  initscr();
  if (0 && has_colors()) {
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
      {".", "drop one element"},
  };

  while (1) {
    erase();
    mvprintw(0, 0, "> ");
    calc.printers[calc.current_printer].func(stack, head);
    refresh();
    mvgetstr(0, 2, buffer);

    if (*buffer == 0) {
      continue;
    }

    char _buffer[80] = {0};
    if (strchr(buffer, ' ')) {
      memcpy(_buffer, buffer, strchr(buffer, ' ') - buffer);
    } else {
      memcpy(_buffer, buffer, strlen(buffer));
    }

    if (strcmp(_buffer, "include") == 0) {
      char *argstart = strchr(buffer, ' ') + 1;
      char arg[90] = {0};
      if (*argstart == '/') {
        snprintf(arg, 90, "%s.so", argstart);
      } else {
        snprintf(arg, 90, "./%s.so", argstart);
      }

      int start_count = calc.op_count;
      if (load_module(&calc, arg)) {
        mvprintw(0, 0, "loaded %d operations from module %s", calc.op_count - start_count, arg);
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
      refresh();
      getch();

    } else if (strcmp(buffer, ".") == 0) {
      if (head < 1) {
        eprint("'%s' NEEDS 1 ARG", buffer);
        continue;
      }
      head--;

    } else if (('0' <= buffer[0] && buffer[0] <= '9') || ((buffer[0] == '.' || buffer[0] == '-') && buffer[1] != '\0')) {
      stack[head++] = strtod(buffer, NULL);

    } else {
      bool is_found = false;
      for (int i = 0; i < calc.op_count; ++i) {
        for (int j = 0; j < MAX_KW_COUNT && calc.operations[i].kws[j]; ++j) {
          if (strcmp(calc.operations[i].kws[j], buffer) == 0) {
            is_found = true;
            if (head < calc.operations[i].nargs) {
              eprint("'%s' NEEDS AT LEAST %d ARGS", buffer, calc.operations[i].nargs);
              goto found;
            }
            calc.operations[i].func(stack, &head);
            goto found;
          }
        }
      }
    found:
      if (!is_found) {
        eprint("operation not found: '%s'", buffer);
        continue;
      }
    }
  }

  endwin();

  for (int i = 0; i < calc.module_count; ++i) {
    dlclose(calc.modules[i]);
  }

  return 0;
}
