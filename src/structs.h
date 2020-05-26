#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>

typedef struct machineState {
    uint8_t *memory;
    uint32_t *registers;
    bool has_instruction;
} machineState;

typedef struct shiftedRegister {
    uint32_t operand2, carryBit;
} shiftedRegister;

#endif
