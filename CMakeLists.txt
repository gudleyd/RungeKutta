cmake_minimum_required(VERSION 3.12)
project(RungeKutta)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

add_executable(RungeKutta main.cpp src/expression/Expression.cpp src/expression/Expression.h src/expression/Tokens.h src/utils/utils.cpp src/utils/utils.h src/runge-kutta/RungeKuttaMethods.h test/Tests.h test/RunTests.h RungeKutta.h)

if(UNIX)
    target_link_libraries(RungeKutta dl)
endif()