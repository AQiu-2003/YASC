cmake_minimum_required(VERSION 3.10)
project(YASC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置构建类型为 Debug，以便包含调试信息
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(
        parser
        src/parser.y
        ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
        COMPILE_FLAGS "-d"
)
flex_target(
        scanner
        src/scanner.l
        ${CMAKE_CURRENT_BINARY_DIR}/scanner.yy.c
)
add_flex_bison_dependency(scanner parser)

include_directories(include)

add_executable(
        YASC
        ${BISON_parser_OUTPUTS}
        ${FLEX_scanner_OUTPUTS}
        src/main.c
        src/symbol.c
        src/analyzer.c
        src/utils.c
)
