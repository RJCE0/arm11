#ifndef CONSTANTS
#define CONSTANTS

#define NUM_INSTRUCTION 6
// including bal
#define NUM_OPCODE 24
#define SHIFT_COND 28
#define MAX_ARG_SIZE 5
#define MULTIPLY_CONST 0x90
#define SDT_CONST 0x4000000
#define BRANCH_CONST 0xA000000
#define AL_COND 0xE0000000
#define MAX_LINE 511

#define DPI_S_BIT 0x100000
#define IMM_BIT 0x2000000
#define SDT_P_BIT 0x1000000
#define SDT_U_BIT 0x800000

typedef enum {
    DPI = 0,
    MI = 1,
    SDTI = 2,
    BI = 3,
    HALT = 4,
} instructionType;

typedef enum {
    ADD = 4,
    SUB = 2,
    RSB = 3,
    AND = 0,
    EOR = 1,
    ORR = 12,
    MOV = 13,
    TST = 8,
    TEQ = 9,
    CMP = 10
} dpiType;

typedef enum {
    MUL = 0,
    MLA = 1
} multiplyType;

typedef enum {
    STR = 0,
    LDR = 1
} sdtType;

typedef enum {
    BEQ = 0,
    BNE = 1,
    BGE = 10,
    BLT = 11,
    BGT = 12,
    BLE = 13,
    B = 14
} branchType;

typedef enum {
    LSL,
    LSR,
    ASR,
    ROR
} shiftType;

#endif
