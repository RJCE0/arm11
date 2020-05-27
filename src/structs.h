#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdlib.h>

typedef struct machineState {
    uint8_t *memory;
    uint32_t *registers;
} machineState;

typedef struct shiftedRegister {
    uint32_t operand2;
    uint32_t carryBit;
} shiftedRegister;

#endif
