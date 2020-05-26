//
// Created by Tyrell Duku on 21/05/2020.
//
#include <stdlib.h>
#include "constants.h"
typedef struct state_of_machine {
    uint8_t *memory;
    uint32_t *registers;
    bool has_instruction;
};
