#ifndef MODULE_H__
#define MODULE_H__

#include <ncurses.h>

#define MAX_KW_COUNT   4
#define MAX_OPERATIONS 2048
#define MAX_PRINTERS   16
#define MAX_MODULES    16

typedef struct {
  char *start;
  int len;
} sv_t;

#define SV_FMT            "%*.*s"
#define SV_UNPACK(__sv__) (__sv__.len), (__sv__.len), (__sv__.start)

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

typedef struct trie_t_ {
  int index;
  struct trie_t_ *children[128];
} trie_t;

typedef struct {
  op_t operations[MAX_OPERATIONS];
  int op_count;
  printer_t printers[MAX_PRINTERS];
  int printer_count;
  int current_printer;
  void *modules[MAX_MODULES];
  int module_count;
  trie_t op_trie;
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

sv_t sv_from_cstr(char *s);
bool sv_eq(sv_t a, sv_t b);

int trie_search(trie_t *node, sv_t sv);
void trie_add(trie_t *trie, sv_t key, int index);

void load(calc_t *calc);
void add_op(calc_t *calc, op_t op);
void add_printer(calc_t *calc, char *name, printer_func_t func);

#endif // MODULE_H__

#ifdef MODULE_IMPL

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

sv_t sv_from_cstr(char *s) {
  return (sv_t){s, strlen(s)};
}

bool sv_eq(sv_t a, sv_t b) {
  if (a.len != b.len) {
    return false;
  }
  return memcmp(a.start, b.start, a.len) == 0;
}

int trie_search(trie_t *node, sv_t key) {
  assert(node);

  trie_t *next = node->children[(int)key.start[0]];

  if (key.len == 0) {
    return node->index;
  } else if (next) {
    return trie_search(next, (sv_t){key.start + 1, key.len - 1});
  } else {
    return -1;
  }
}

void trie_add(trie_t *node, sv_t key, int index) {
  assert(node);

  if (key.len == 0) {
    assert(node->index == -1);
    node->index = index;
  } else if (node->children[(int)key.start[0]]) {
    trie_add(node->children[(int)key.start[0]], (sv_t){key.start + 1, key.len - 1}, index);
  } else {
    trie_t *new = calloc(1, sizeof(trie_t));
    assert(new);
    new->index = -1;
    node->children[(int)key.start[0]] = new;
    trie_add(node, key, index);
  }
}

void add_op(calc_t *calc, op_t op) {
  assert(calc);
  assert(calc->op_count + 1 < MAX_OPERATIONS);
  calc->operations[calc->op_count++] = op;

  for (int i = 0; i < MAX_KW_COUNT && op.kws[i]; ++i) {
    trie_add(&calc->op_trie, sv_from_cstr(op.kws[i]), calc->op_count - 1);
  }
}

void add_printer(calc_t *calc, char *name, printer_func_t func) {
  assert(calc);
  assert(calc->printer_count + 1 < MAX_PRINTERS);
  calc->printers[calc->printer_count++] = (printer_t){name, func};
}

#endif // MODULE_IMPL
