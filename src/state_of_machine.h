//
// Created by Tyrell Duku on 21/05/2020.
//
#include "stdlib.h"
#include "constants.h"
struct state_of_machine {
    char memory[NO_ADDRESSES];
    uint32_t registers[NUM_OF_REGISTERS];
    bool has_instruction;
};
