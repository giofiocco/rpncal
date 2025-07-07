#ifndef MODULE_H__
#define MODULE_H__

#include <ncurses.h>

#define MAX_KW_COUNT 16

typedef struct {
  char *kws[MAX_KW_COUNT];
  int nargs;
  char *description;
  void (*func)(double *stack, int *head);
} op_t;

typedef void (*load_func_t)(op_t *operations, int *op_count);

void load(op_t *operations, int *op_count);

#endif // MODULE_H__
