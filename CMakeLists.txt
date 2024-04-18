cmake_minimum_required(VERSION 3.27)
project(compiler_test C)

set(CMAKE_C_STANDARD 11)

add_executable(compiler_test syntax_tree.c lex.yy.c syntax.tab.c)
