# YASC - Yet Another SNL Compiler

Language: C (with flex/bison)

Start Time: 2024.04.02

End Time: 2024.04.23

## Compiler Progress

- [x] Lexical analysis
- [x] Syntax analysis
- [x] Semantic analysis
- [ ] Intermediate code generation

## Environment

Tools to be installed:

- **flex** 2.6.4 Apple(flex-34)
- **bison** 3.8.2 (GNU Bison)
- **cmake** version 3.28.3

Test System:

- [x] **macOS** Sonoma 14.2.1
- [x] **linux** Ubuntu 22.04.4 LTS
- [x] **windows** Windows 10/11 (with MinGW/GCC)

## Build & Run

```bash
mkdir build
cd build
cmake ..
make
./yasc [input_file] [--print-token] [--print-ast]
# --print-token: print tokens
# --print-ast: print abstract syntax tree
```