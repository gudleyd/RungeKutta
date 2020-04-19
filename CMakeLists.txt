cmake_minimum_required(VERSION 3.15)
project(RungeKutta)

set(CMAKE_CXX_STANDARD 17)

add_executable(RungeKutta main.cpp src/parser/Expression.cpp src/parser/Expression.cpp src/parser/Expression.h src/parser/Tokens.h src/utils/utils.cpp src/utils/utils.h)