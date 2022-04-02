cmake_minimum_required(VERSION 3.21)
project(circuit C)

set(CMAKE_C_STANDARD 11)

add_executable(circuit main.c circuit.c circuit.h)
