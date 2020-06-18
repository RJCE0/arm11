#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include <stdint.h>
#include "constants.h"
#include "instructionTypes.h"

typedef struct {
    cpsrFlags flag;
    bool set: 1;
} flagChange;

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint32_t registers[NUM_OF_REGISTERS];
    uint32_t fetched;
    decodedInstruction *instructionAfterDecode;
    bool fetchedInstr;
} machineState;

#endif
