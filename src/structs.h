#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdlib.h>
#include <stdint.h>
#include "constants.h"
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
    uint32_t operand2;
    uint32_t carryBit;
} shiftedRegister;

typedef struct  {
    instructionOpcode opcode : 4;
    uint16_t operand2 : 12;
    uint8_t rn : 4;
    uint8_t rd : 4;

    bool immediate;
    bool setBit;
} dataProcessingInstruction;

typedef struct {
    uint8_t rn : 4;
    uint8_t rd : 4;
    uint8_t rs : 4;
    uint8_t rm : 4;

    bool accumBit;
    bool setBit;
} multiplyInstruction;

typedef struct {
    uint16_t rn : 4;
    uint16_t rd : 4;
    uint32_t offset : 12;

    bool upBit;
    bool loadBit;
    bool indexingBit;
    bool immediate;
} sdtInstruction;


typedef struct {
    uint32_t offset : 24;
} branchInstruction;

typedef struct {
    uint8_t rn : 4;
    uint8_t rd : 4;
    uint8_t rs : 4;
    uint8_t rm : 4;

    bool accumBit;
    bool setBit;
}nullInstruction;

typedef struct {
    instructionType type : 3;
    uint8_t condCode: 4;
    union {
        dataProcessingInstruction dpi;
        multiplyInstruction mi;
        sdtInstruction sdti;
        branchInstruction bi;
        nullInstruction ni;
    } u;
} decodedInstruction;

/*const decodedInstruction null_instruction = {
    .type = NULL_I,
    .condCode =
};*/
typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint32_t registers[NUM_OF_REGISTERS];
    uint32_t fetched;
    decodedInstruction *instructionAfterDecode;
/*
    decodedInstruction *instructionAfterExecute;
*/
    bool fetchedInstr;
    bool decodedInstr;
} machineState;


#endif
