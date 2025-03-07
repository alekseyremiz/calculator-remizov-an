# calculator-remizov-an 

A simple arithmetic expression parser and evaluator written in C.  
Supports `+`, `-`, `*`, `/`, and parentheses `()` with integer and floating-point modes.

## Features
- **Integer & Floating-Point Modes** (`--float` flag for floating-point calculations)
- **Handles Parentheses** for correct order of operations
- **Whitespace Tolerant** – input can contain spaces freely
- **Safe Evaluations** – prevents overflow and division by zero

## Usage
Compile the program:
```sh
gcc -o calculator.exe calculator.c -lm
