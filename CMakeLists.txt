cmake_minimum_required(VERSION 3.16)
project(arm11_37)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(arm11_37
        src/assemble.c
        src/constants.h
        src/emulate.c
        src/emulate.h
        src/Makefile
        src/structs.h
        src/test.bin)
