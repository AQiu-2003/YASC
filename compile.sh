#!/bin/bash

# Check the operating system
os=$(uname -s)

# Compile Bison and Flex files
 bison -d syntax.y
 flex lexial.l

# Compile the parser
if [ "$os" == "Darwin" ]; then
    gcc syntax.tab.c syntax_tree.c lex.yy.c -ly -o parser
    if [ $? -eq 0 ]; then
        echo "Compilation successful"
    else
        echo "Compilation failed"
    fi
elif [ "$os" == "Linux" ]; then
    gcc syntax.tab.c syntax_tree.c lex.yy.c -Xlinker -zmuldefs -o parser
    echo "Parser compiled successfully."
    if [ $? -eq 0 ]; then
        echo "Compilation successful"
    else
        echo "Compilation failed"
    fi
else
    echo "Unsupported operating system: $os"
    echo "Compilation failed"
    exit 1
fi


