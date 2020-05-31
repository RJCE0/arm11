#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdlib.h>
#include <stdint.h>
#include "constants.h"
#include "instructionTypes.h"
/* Idea for pipeline:
 *  - point to the previous step in the cycle so that we don't lose it.
 *  - Start off by initialising it null for the first cycle.
 *  - Branch:
 *      - Skips to new place -- all instructions already in pipeline must be cleared.
 1st cycle -- only fetch (initialise others with null)
After execution, move pointers back one -- change what they're pointed to with each cycle.
BRANCH -- clear pipeline

*/

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint32_t registers[NUM_OF_REGISTERS];
    uint32_t fetched;
    decodedInstruction *instructionAfterDecode;
    bool fetchedInstr;
    bool decodedInstr;
} machineState;


#endif
