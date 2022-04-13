cmake_minimum_required(VERSION 3.21)
project(circuit C)

set(CMAKE_C_STANDARD 11)

link_libraries(SDL2 SDL2_ttf)

add_executable(circuit main.c main.h circuit.c circuit.h gui.c gui.h)
