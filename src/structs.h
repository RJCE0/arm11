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

typedef struct dataProcessingInstruction {
    enum instructionOpcodes opcode;
    uint32_t operand2;
    uint8_t rn;
    uint8_t rd;

    bool immediate_operand;
    bool set_cond;
} dpi;

typedef struct multiplyInstruction {

    uint8_t rn;
    uint8_t rd;
    uint8_t rs;
    uint8_t rm;

    bool accu_cond;
    bool set_cond;
} mi;

typedef struct sdtInstruction {

    uint16_t rn;
    uint16_t rd;
    uint16_t offset;

    bool up_cond;
    bool load_cond;
    bool indexing_cond;
    bool offset_cond;
} sdti;

typedef struct branchInstruction {
    uint32_t offset;
} bi;

typedef union di {
    dpi dpi_instruction;
    mi m_instruction;
    sdti sdt_instruction;
    bi b_instruction;
} di;

typedef struct machineState {
    uint8_t *memory;
    uint32_t *registers;
    uint32_t fetched;
    uint32_t toBeDecoded;
    di decodedInstruction;
    instructionType before;
    di toBeExecuted;
    instructionType after;

} machineState;

#endif
