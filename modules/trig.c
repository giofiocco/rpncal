#include <assert.h>
#include <math.h>

#include "../module.h"

void sin_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = sin(a);
}

void cos_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = cos(a);
}

void deg(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = a * 180.0 / M_PI;
}

void rad(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = a * M_PI / 180.0;
}

void load(calc_t *calc) {
  assert(calc);
  assert(calc->op_count + 4 < MAX_OPERATIONS);
  calc->operations[calc->op_count++] = (op_t){{"sin"}, 1, "sin(a) with a in radians", sin_};
  calc->operations[calc->op_count++] = (op_t){{"cos"}, 1, "cos(a) with a in radians", cos_};
  calc->operations[calc->op_count++] = (op_t){{"deg"}, 1, "a to degrees", deg};
  calc->operations[calc->op_count++] = (op_t){{"rad"}, 1, "a to radians", rad};
}
