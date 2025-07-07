# RPNcal

A TUI RPN calculator extesible.
It operates on `double`s, can input operations or numbers.

- entring a number push it to the stack.
- `include <name>` includes the module `name` as a .so file (`./name.so` or `name.so` if is an absolute path).
- `quit` or `exit` ends the program.
- `?` or `help` prints all the operations.

# Make a module

A module is a .so file that have a `void load(op_t *operation, int *op_count)` function (include `module.h` for definitions) that will push (`operation[(*op_count) ++] = ...`) new operations defined as:

- kws: list of kws to call that operations.
- desc: description
- nargs: minimum number of arguments for the operation.
- func: pointer to the function that implement the operation.

The function for the operation has to be `void func(double *stack, int *head)`
`stack[--(*head)]` or `stack[(*head)++]` can be used to pop/push respectively numbers to the stack.

To compile a module:

```sh
cc -c -fpic -o module.o module.c
cc -shared -o module.so module.o
```
