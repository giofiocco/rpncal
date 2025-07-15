#ifndef MODULE_H__
#define MODULE_H__

#include <ncurses.h>

#define MAX_KW_COUNT   4
#define MAX_OPERATIONS 2048
#define MAX_PRINTERS   16
#define MAX_MODULES    16

typedef void (*op_func_t)(double *stack, int *head);
typedef void (*printer_func_t)(double *stack, int count);

typedef struct {
  char *kws[MAX_KW_COUNT];
  int nargs;
  char *description;
  op_func_t func;
} op_t;

typedef struct {
  char *name;
  printer_func_t func;
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

void load(calc_t *calc);
void add_op(calc_t *calc, op_t op);
void add_printer(calc_t *calc, char *name, printer_func_t func);

#endif // MODULE_H__

#ifdef MODULE_IMPL

#include <assert.h>

void add_op(calc_t *calc, op_t op) {
  assert(calc);
  assert(calc->op_count + 1 < MAX_OPERATIONS);
  calc->operations[calc->op_count++] = op;
}

void add_printer(calc_t *calc, char *name, printer_func_t func) {
  assert(calc);
  assert(calc->printer_count + 1 < MAX_PRINTERS);
  calc->printers[calc->printer_count++] = (printer_t){name, func};
}

#endif // MODULE_IMPL
