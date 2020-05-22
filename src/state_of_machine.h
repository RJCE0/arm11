//
// Created by Tyrell Duku on 21/05/2020.
//
#include "stdlib.h"
struct state_of_machine {
    char memory[NUM_ADDRESSES];
    uint32_t registers[NUM_OF_REGISTERS];
    bool has_instruction;
};
