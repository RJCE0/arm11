#ifndef CONSTANTS_H
#define CONSTANTS_H


#define WORD_SIZE 32
#define MEMORY_SIZE 65536
#define NUM_OF_REGISTERS 17
#define CPSR_REG 16
#define PC_REG 15
#define SHIFT_COND 28
#define SE_32 0xFF000000
#define WORD_SIZE_IN_BYTES 4
#define SIGN_32_BIT 0x80000000

enum condition {
    EQ = 0,     /* 0000 */
    NE = 1,     /* 0001 */
    GE = 10,    /* 1010 */
    LT = 11,    /* 1011 */
    GT = 12,    /* 1100 */
    LE = 13,    /* 1101*/
    AL = 14     /* 1110 */
};

typedef enum {
    V_FLAG = 1,  /* 0001 */
    C_FLAG = 2,  /* 0010 */
    Z_FLAG = 4,  /* 0100 */
    N_FLAG = 8   /* 1000 */
} cpsrFlags;

typedef enum {
    AND = 0,    /* 0000 */
    EOR = 1,    /* 0001 */
    SUB = 2,    /* 0010 */
    RSB = 3,    /* 0011 */
    ADD = 4,    /* 0100 */
    TST = 8,    /* 1000 */
    TEQ = 9,    /* 1001 */
    CMP = 10,   /* 1010 */
    ORR = 12,   /* 1100 */
    MOV = 13    /* 1101 */
} instructionOpcode;

enum shiftCodes {
    LOGICAL_LEFT = 0,
    LOGICAL_RIGHT = 1,
    ARITH_RIGHT = 2,
    ROTATE_RIGHT = 3
};

typedef enum {
    DATA_PROCESSING,
    MULTIPLY,
    SINGLE_DATA_TRANSFER,
    BRANCH,
    ZERO,
    NULL_INSTR
} instructionType;


#endif
