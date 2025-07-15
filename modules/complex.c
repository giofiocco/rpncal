#include <assert.h>
#include <math.h>

#define MODULE_IMPL
#include "../module.h"

void printer(double *stack, int count) {
  if (count == 0) {
    return;
  }
  int j = 0;
  if (count % 2 != 0) {
    mvprintw(j + 1, 0, "%d: %c%E + %EI", j, stack[count - 1] > 0 ? ' ' : '-', fabs(stack[count - 1]), 0.0);
    j++;
  }
  for (int i = count - 1 - count % 2; i >= 0; i -= 2) {
    double x = stack[i];
    double y = stack[i - 1];
    mvprintw(j + 1, 0, "%d: %c%E %c %EI", j, x > 0 ? ' ' : '-', fabs(x), y > 0 ? '+' : '-', fabs(y));
    j++;
  }
}

void add(double *stack, int *head) {
  double x1 = stack[--(*head)];
  double y1 = stack[--(*head)];
  double x2 = stack[--(*head)];
  double y2 = stack[--(*head)];
  stack[(*head)++] = y1 + y2;
  stack[(*head)++] = x1 + x2;
}

void sub(double *stack, int *head) {
  double x1 = stack[--(*head)];
  double y1 = stack[--(*head)];
  double x2 = stack[--(*head)];
  double y2 = stack[--(*head)];
  stack[(*head)++] = y2 - y1;
  stack[(*head)++] = x2 - x1;
}

void mul(double *stack, int *head) {
  double x1 = stack[--(*head)];
  double y1 = stack[--(*head)];
  double x2 = stack[--(*head)];
  double y2 = stack[--(*head)];
  stack[(*head)++] = y1 * x2 + x1 * y2;
  stack[(*head)++] = x1 * x2 - y1 * y2;
}

void conj_(double *stack, int *head) {
  double x = stack[--(*head)];
  stack[*head - 1] *= -1;
  stack[(*head)++] = x;
}

void load(calc_t *calc) {
  add_printer(calc, "complex", printer);
  add_op(calc, (op_t){{"c+"}, 4, "complex +", add});
  add_op(calc, (op_t){{"c-"}, 4, "complex -", sub});
  add_op(calc, (op_t){{"c*"}, 4, "complex *", mul});
  add_op(calc, (op_t){{"c_"}, 2, "complex conj", conj_});
}
