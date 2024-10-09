# 238P OS Project1: JIT-Compiled-Expression-Evaluator

## Overview  
This program dynamically compiles and executes a mathematical expression. It accepts a mathematical expression (a string containing symbols: +, -, *, /, (, ), value). The program generates a C source code file based on this expression and uses the fork(), execv(), and waitpid() system calls to invoke the GCC compiler, producing a shared object (dynamically loadable module). The compiled module is then loaded into memory using dlopen(), and the result of the expression is evaluated by looking up the relevant symbol with dlsym() and executing the compiled code.

## Usage
```
make
./cs238 "expression"
```
The output should be
```
Sigmoid value: "value"
Original value: "value"
```

## Special feature
The Sigmoid funcion is implemented by passing its address to the dynamically generated C code. The function is then called from the generated code using the address to compute the sigmoid value for the expression. 

## Known issue
While the project description mentions support for the "%" symbol, this program is currently unable to process expressions containing it.
