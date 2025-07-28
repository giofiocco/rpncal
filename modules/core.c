#include <assert.h>
#include <math.h>

#define MODULE_IMPL
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

void pow_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[*head - 1] = pow(stack[*head - 1], a);
}

void ln(double *stack, int *head) {
  stack[*head - 1] = log(stack[*head - 1]);
}

void log10_(double *stack, int *head) {
  stack[*head - 1] = log10(stack[*head - 1]);
}

void log2_(double *stack, int *head) {
  stack[*head - 1] = log2(stack[*head - 1]);
}

void log_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[*head - 1] = log(stack[*head - 1]) / log(a);
}

void load(calc_t *calc) {
  add_op(calc, (op_t){{"+"}, 2, "a + b", add});
  add_op(calc, (op_t){{"-"}, 2, "b - a", sub});
  add_op(calc, (op_t){{"*"}, 2, "a * b", mul});
  add_op(calc, (op_t){{"/"}, 2, "b / a", div_});
  add_op(calc, (op_t){{"neg", "_"}, 1, "-a", neg});
  add_op(calc, (op_t){{"oo"}, 1, "1 / a", oo});
  add_op(calc, (op_t){{"sum"}, 2, "sum first a elements", sum});
  add_op(calc, (op_t){{"mean"}, 2, "mean of first a elements", mean});
  add_op(calc, (op_t){{"pi"}, 0, "pi", pi});
  add_op(calc, (op_t){{"exp"}, 0, "exp(a)", exp_});
  add_op(calc, (op_t){{"sqrt"}, 0, "sqrt(a)", sqrt_});
  add_op(calc, (op_t){{"^2"}, 0, "a ^ 2", square});
  add_op(calc, (op_t){{"pow", "^"}, 2, "b ^ a", pow_});
  add_op(calc, (op_t){{"ln"}, 1, "ln(a)", ln});
  add_op(calc, (op_t){{"log10"}, 1, "log_10(a)", log10_});
  add_op(calc, (op_t){{"log2"}, 1, "log_2(a)", log2_});
  add_op(calc, (op_t){{"log"}, 2, "log_a(b)", log_});
}
