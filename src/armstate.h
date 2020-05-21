//
// Created by Tyrell Duku on 21/05/2020.
//
#include "stdlib.h"
#include "constants.h"
#ifndef ARM11_37_ARMSTATE_H
#define ARM11_37_ARMSTATE_H

struct ARMState {
    char memory_locations[NUM_ADDRESSES];
    uint32_t registers[NUM_OF_REGISTERS];
};

#endif //ARM11_37_ARMSTATE_H
