#include <stdio.h>

#include "module.h"

void neg(double *stack, int *head) {
  double a = stack[*head - 1];
  stack[*head - 1] = -a;
}

void oover(double *stack, int *head) {
  double a = stack[*head - 1];
  stack[*head - 1] = 1.0 / a;
}

void load(op_t *operations, int *op_count) {
  operations[(*op_count)++] = (op_t){{"neg", "_"}, 1, "", neg};
  operations[(*op_count)++] = (op_t){{"oo"}, 1, "", oover};
}
