cmake_minimum_required(VERSION 3.16)
project(arm11_37)

set(CMAKE_CXX_STANDARD 14)

add_executable(arm11_37
        doc/Checkpoint.tex
        doc/Makefile
        doc/Report.tex
        programs/gpio.s
        src/assemble.c
        src/emulate.c
        src/Makefile src/state_of_machine.h src/constants.h)
