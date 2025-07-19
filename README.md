# RPNcal

A TUI RPN calculator extesible.
It operates on `double`s.

- entring a number push it to the stack.
- `include <path>` includes the module `path` as a .so file (`./path.so` or `path.so` if is an absolute path).
- `quit` or `exit` ends the program.
- `?` or `help` prints all the operations.

Multiple operations and number can be entered at once.

# Make a module

The template for a new module is:

```c
#define MODULE_IMPL
#include "module.h"

...

void load(calc_t *calc) {
    ...
}

```

in `load` should be called `add_op(calc, (op_t){...});` for each operation
and `add_printer(calc, "...", ...);` for each printer.

To compile a module:

```sh
cc -c -fpic -o module.o module.c
cc -shared -o module.so module.o
```

## Operations

Operations are defined as:

- kws: list of kws to call that operations.
- desc: description
- nargs: minimum number of arguments for the operation.
- func: pointer to the function that implement the operation.

the litteral form can be `(op_t){{"kw1", "kw2", ...}, ..., "...", ...}`.

The function that implement the logic has to be: `void sub(double *stack, int *head)`

poping a number: `double a = stack[--(*head)];`

pushing a number: `stack[(*head)++] = b;`

## Printer

The printer function has to be: `void printer(double *stack, int count)`
and visualize the stack using `ncurses`'s functions leaving the first rowfor the input.
