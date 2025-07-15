#include <assert.h>
#include <math.h>

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

void div_(double *stack, int *head) {
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

void pi(double *stack, int *head) {
  stack[(*head)++] = M_PI;
}

void exp_(double *stack, int *head) {
  stack[*head - 1] = exp(stack[*head - 1]);
}

void sqrt_(double *stack, int *head) {
  stack[*head - 1] = sqrt(stack[*head - 1]);
}

void square(double *stack, int *head) {
  stack[*head - 1] *= stack[*head - 1];
}

void load(calc_t *calc) {
  assert(calc);
  assert(calc->op_count + 12 < MAX_OPERATIONS);
  calc->operations[calc->op_count++] = (op_t){{"+"}, 2, "a + b", add};
  calc->operations[calc->op_count++] = (op_t){{"-"}, 2, "b - a", sub};
  calc->operations[calc->op_count++] = (op_t){{"*"}, 2, "a * b", mul};
  calc->operations[calc->op_count++] = (op_t){{"/"}, 2, "b / a", div_};
  calc->operations[calc->op_count++] = (op_t){{"neg", "_"}, 1, "-a", neg};
  calc->operations[calc->op_count++] = (op_t){{"oo"}, 1, "1 / a", oo};
  calc->operations[calc->op_count++] = (op_t){{"sum"}, 2, "sum first a elements", sum};
  calc->operations[calc->op_count++] = (op_t){{"mean"}, 2, "mean of first a elements", mean};
  calc->operations[calc->op_count++] = (op_t){{"pi"}, 0, "pi", pi};
  calc->operations[calc->op_count++] = (op_t){{"exp"}, 0, "exp(a)", exp_};
  calc->operations[calc->op_count++] = (op_t){{"sqrt"}, 0, "sqrt(a)", sqrt_};
  calc->operations[calc->op_count++] = (op_t){{"^2"}, 0, "a ^ 2", square};
}
