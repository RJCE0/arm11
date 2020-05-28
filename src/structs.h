#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdlib.h>
#include "constants.h"

typedef struct machineState {
    uint8_t *memory;
    uint32_t *registers;
    instruct decoded_instruction;
    
} machineState;

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

typedef union instruction {
    dpi dpi_instruction;
    mi m_instruction;
    sdti sdt_instruction;
    bi b_instruction;
} instruct;

#endif
