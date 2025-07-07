#include <assert.h>
#include <dlfcn.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "module.h"

#define eprint(fmt, ...)                            \
  attron(COLOR_PAIR(1));                            \
  mvprintw(0, 0, "ERROR: " fmt "...", __VA_ARGS__); \
  attroff(COLOR_PAIR(1));                           \
  refresh();                                        \
  getch();

int load_module(void **modules, int *module_count, char *module_path, op_t *operations, int *op_count) {
  modules[(*module_count)++] = dlopen(module_path, RTLD_NOW);
  if (!modules[(*module_count) - 1]) {
    eprint("module not loaded: '%s': %s", module_path, dlerror());
    return 0;
  }

  ((load_func_t)dlsym(modules[*module_count - 1], "load"))(operations, op_count);

  return 1;
}

void sum(double *stack, int *head) {
  int a = stack[*head - 1];
  if (*head < a) {
    eprint("sum %d expects %d elements in the stack", a, a);
    return;
  }
  --(*head);
  double sum = 0;
  for (int i = 0; i < a; ++i) {
    sum += stack[--(*head)];
  }
  stack[(*head)++] = sum;
}

int main() {
  if (!isatty(1)) {
    fprintf(stderr, "ERROR: unable to run in this terminal\n");
    return 1;
  }

  op_t operations[2048] = {0};
  int op_count = 0;
  double stack[2048] = {0};
  int head = 0;
  char buffer[80] = {0};

  operations[op_count++] = (op_t){{"sum"}, 2, "sum first a elements from the stack", sum};

  void *modules[2048] = {0};
  int module_count = 0;

  char *helps[][2] = {
      {"include <name>", "include module"},
      {"quit | exit", "end program"},
      {"? | help", "print all operations"},
      {"+", "a + b"},
      {"-", "b - a"},
      {"*", "a * b"},
      {"/", "b / a"},
      {".", "drop one element"},
      {"neg | _", "-a"},
      {"oo", "1 / a"}};

  char *autoload_modules[] = {
      "./modules/trig.so",
  };

  initscr();
  if (0 && has_colors()) {
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
  }

  int autoload_count = sizeof(autoload_modules) / sizeof(autoload_modules[0]);
  for (int i = 0; i < autoload_count; ++i) {
    load_module(modules, &module_count, autoload_modules[i], operations, &op_count);
  }

  while (1) {
    erase();
    mvprintw(0, 0, "> ");
    for (int i = 0; i < head; ++i) {
      if (stack[i] < 0) {
        mvprintw(head - i, 0, "%d: %E    %f\n", head - i - 1, stack[i], stack[i]);
      } else {
        mvprintw(head - i, 0, "%d:  %E    %f\n", head - i - 1, stack[i], stack[i]);
      }
    }
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

      int start_count = op_count;
      if (load_module(modules, &module_count, arg, operations, &op_count)) {
        mvprintw(0, 0, "LOADED %d OPERATIONS FROM MODULE %s", op_count - start_count, arg);
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

      for (int i = 0; i < op_count; ++i) {
        move(count + i, 0);
        int w = 0;
        for (int j = 0; j < MAX_KW_COUNT && operations[i].kws[j]; ++j) {
          if (j > 0) {
            printw(" | ");
            w += 3;
          }
          printw("%s", operations[i].kws[j]);
          w += strlen(operations[i].kws[i]);
        }
        maxw = w > maxw ? w : maxw;
      }

      for (int i = 0; i < count; ++i) {
        mvprintw(i, maxw + 2, "%s", helps[i][1]);
      }

      for (int i = 0; i < op_count; ++i) {
        mvprintw(count + i, maxw + 2, "%s", operations[i].description);
      }

      refresh();
      getch();
    } else if (strcmp(buffer, "+") == 0
               || strcmp(buffer, "-") == 0
               || strcmp(buffer, "*") == 0
               || strcmp(buffer, "/") == 0) {
      if (head < 2) {
        eprint("'%s' NEEDS 2 ARGS", buffer);
        continue;
      }
      double a = stack[--head];
      double b = stack[--head];

      switch (buffer[0]) {
        case '+': stack[head++] = a + b; break;
        case '-': stack[head++] = b - a; break;
        case '*': stack[head++] = a * b; break;
        case '/': stack[head++] = b / a; break;
      }

    } else if (strcmp(buffer, "oo") == 0) {
      if (head < 1) {
        eprint("'%s' NEEDS 1 ARG", buffer);
        continue;
      }
      double a = stack[--head];
      stack[head++] = 1.0 / a;

    } else if (strcmp(buffer, "neg") == 0 || strcmp(buffer, "_") == 0) {
      if (head < 1) {
        eprint("'%s' NEEDS 1 ARG", buffer);
        continue;
      }
      double a = stack[--head];
      stack[head++] = -a;

    } else if (strcmp(buffer, ".") == 0) {
      if (head < 1) {
        eprint("'%s' NEEDS 1 ARG", buffer);
        continue;
      }
      --head;

    } else if (('0' <= buffer[0] && buffer[0] <= '9') || buffer[0] == '-' || (buffer[0] == '.' && buffer[1] != '\0')) {
      stack[head++] = strtod(buffer, NULL);

    } else {
      bool is_found = false;
      for (int i = 0; i < op_count; ++i) {
        for (int j = 0; j < MAX_KW_COUNT && operations[i].kws[j]; ++j) {
          if (strcmp(operations[i].kws[j], buffer) == 0) {
            is_found = true;
            if (head < operations[i].nargs) {
              eprint("'%s' NEEDS AT LEAST %d ARGS", buffer, operations[i].nargs);
              goto found;
            }
            operations[i].func(stack, &head);
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

  for (int i = 0; i < module_count; ++i) {
    dlclose(modules[i]);
  }

  return 0;
}
