#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdlib.h>
#include "constants.h"
/* Idea for pipeline:
 *  - point to the previous step in the cycle so that we don't lose it.
 *  - Start off by initialising it null for the first cycle.
 *  - Branch:
 *      - Skips to new place -- all instructions already in pipeline must be cleared.

/* 1st cycle -- only fetch (initialise others with null)
After execution, move pointers back one -- change what they're pointed to with each cycle.
BRANCH -- clear pipeline

*/



typedef enum instructionType {
    ZERO,
    DATA_PROCESSING,
    MULTIPLY,
    SINGLE_DATA_TRANSFER,
    BRANCH
}instructionType;

typedef struct shiftedRegister {
    uint32_t operand2;
    uint32_t carryBit;
} shiftedRegister;

typedef struct  {
    instructionOpcode opcode : 4;
    uint32_t operand2;
    uint8_t rn : 4;
    uint8_t rd : 4;

    bool carryCond;
    bool setCond;
} dataProcessingInstruction;

typedef struct {
    uint8_t rn : 4;
    uint8_t rd : 4;
    uint8_t rs : 4;
    uint8_t rm : 4;

    bool accu_cond;
    bool set_cond;
} multiplyInstruction;

typedef struct {

    uint16_t rn : 4;
    uint16_t rd : 4;
    uint32_t offset;

    bool up_cond;
    bool load_cond;
    bool indexing_cond;
    bool offset_cond;
} sdtInstruction;

/*
const dpi zeroInstruction = {
    .opcode = 0,
    .rn = 0,
    .rd = 0,
    .operand2 = 0,
    .carryCond = false,
    .setCond = false
};
*/

typedef struct branchInstruction {
    uint32_t offset : 24;
} branchInstruction;

typedef struct {
    instructionType type : 3;
    uint8_t cond_code: 4;
    union {
        dataProcessingInstruction dpi;
        multiplyInstruction mi;
        sdtInstruction sdti;
        branchInstruction bi;
    };
} decodedInstruction;


typedef struct machineState {
    uint8_t *memory;
    uint32_t *registers;
    uint32_t instructionFetched;
    decodedInstruction *instructionToDecode;
    decodedInstruction *instructionToExecute;

} machineState;


#endif
