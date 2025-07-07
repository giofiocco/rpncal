#ifndef MODULE_H__
#define MODULE_H__

#include <ncurses.h>

#define MAX_KW_COUNT   16
#define MAX_OPERATIONS 2048
#define MAX_PRINTERS   16
#define MAX_MODULES    16

typedef struct {
  char *kws[MAX_KW_COUNT];
  int nargs;
  char *description;
  void (*func)(double *stack, int *head);
} op_t;

typedef struct {
  char *name;
  void (*func)(double *stack, int count);
} printer_t;

typedef struct {
  op_t operations[MAX_OPERATIONS];
  int op_count;
  printer_t printers[MAX_PRINTERS];
  int printer_count;
  int current_printer;
  void *modules[MAX_MODULES];
  int module_count;
} calc_t;

void load(calc_t *calc);

#define eprint(fmt, ...)                            \
  attron(COLOR_PAIR(1));                            \
  mvprintw(0, 0, "ERROR: " fmt "...", __VA_ARGS__); \
  attroff(COLOR_PAIR(1));                           \
  refresh();                                        \
  getch();

#define eprint_(str)                   \
  attron(COLOR_PAIR(1));               \
  mvprintw(0, 0, "ERROR: " str "..."); \
  attroff(COLOR_PAIR(1));              \
  refresh();                           \
  getch();

#endif // MODULE_H__
