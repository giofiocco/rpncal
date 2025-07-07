#include <assert.h>

#include "../module.h"

void add(double *stack, int *head) {
  double a = stack[--(*head)];
  double b = stack[--(*head)];
  stack[(*head)++] = a + b;
}

void sub(double *stack, int *head) {
  double a = stack[--(*head)];
  double b = stack[--(*head)];
  stack[(*head)++] = b - a;
}

void mul(double *stack, int *head) {
  double a = stack[--(*head)];
  double b = stack[--(*head)];
  stack[(*head)++] = a * b;
}

void div(double *stack, int *head) {
  double a = stack[--(*head)];
  double b = stack[--(*head)];
  stack[(*head)++] = b / a;
}

void neg(double *stack, int *head) {
  stack[*head - 1] *= -1;
}

void oo(double *stack, int *head) {
  stack[*head - 1] = 1.0 / stack[*head - 1];
}

void sum(double *stack, int *head) {
  int a = stack[*head - 1];
  if (*head - 1 < a) {
    eprint("expected %d elements in the stack", a + 1);
    return;
  }
  (*head)--;
  double sum = 0;
  for (int i = 0; i < a; ++i) {
    sum += stack[--(*head)];
  }
  stack[(*head)++] = sum;
}

void mean(double *stack, int *head) {
  int a = stack[*head - 1];
  sum(stack, head);
  stack[*head - 1] /= a;
}

void load(calc_t *calc) {
  assert(calc);
  assert(calc->op_count + 8 < MAX_OPERATIONS);
  calc->operations[calc->op_count++] = (op_t){{"+"}, 2, "a + b", add};
  calc->operations[calc->op_count++] = (op_t){{"-"}, 2, "b - a", sub};
  calc->operations[calc->op_count++] = (op_t){{"*"}, 2, "a * b", mul};
  calc->operations[calc->op_count++] = (op_t){{"/"}, 2, "b / a", div};
  calc->operations[calc->op_count++] = (op_t){{"neg", "_"}, 1, "-a", neg};
  calc->operations[calc->op_count++] = (op_t){{"oo"}, 1, "1 / a", oo};
  calc->operations[calc->op_count++] = (op_t){{"sum"}, 2, "sum first a elements", sum};
  calc->operations[calc->op_count++] = (op_t){{"mean"}, 2, "mean of first a elements", mean};
}
