#ifndef MACHINE_STATE
#define MACHINE_STATE

#include <stdlib.h>
#include <stdint.h>
#include "constants.h"
#include "instruction_types.h"

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
