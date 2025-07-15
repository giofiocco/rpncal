#include <assert.h>
#include <math.h>

#define MODULE_IMPL
#include "../module.h"

void sin_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = sin(a);
}

void cos_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = cos(a);
}

void tan_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = tan(a);
}

void asin_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = asin(a);
}

void acos_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = acos(a);
}

void atan_(double *stack, int *head) {
  double a = stack[--(*head)];
  stack[(*head)++] = atan(a);
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
  add_op(calc, (op_t){{"sin"}, 1, "sin(a) with a in radians", sin_});
  add_op(calc, (op_t){{"cos"}, 1, "cos(a) with a in radians", cos_});
  add_op(calc, (op_t){{"tan"}, 1, "tan(a) with a in radians", tan_});
  add_op(calc, (op_t){{"asin"}, 1, "inverse sin(a) in radians", asin_});
  add_op(calc, (op_t){{"acos"}, 1, "inverse cos(a) in radians", acos_});
  add_op(calc, (op_t){{"atan"}, 1, "inverse tan(a) in radians", atan_});
  add_op(calc, (op_t){{"deg"}, 1, "a to degrees", deg});
  add_op(calc, (op_t){{"rad"}, 1, "a to radians", rad});
}
