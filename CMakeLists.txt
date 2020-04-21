cmake_minimum_required(VERSION 3.12)
project(RungeKutta)

set(CMAKE_CXX_STANDARD 17)

add_executable(RungeKutta main.cpp src/expression/Expression.cpp src/expression/Expression.h src/expression/Tokens.h src/utils/utils.cpp src/utils/utils.h)