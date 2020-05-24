//
// Created by Tyrell Duku on 21/05/2020.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H


#define WORD_SIZE 32
#define NO_ADDRESSES 65536
#define NUM_OF_REGISTERS 32
#define CPSR_REG 16
#define PC_REG 15
#define SHIFT_COND 28
#define SIGN_EXTENSION__TO_32 0xFE000000

enum condition {
    EQ = 0,
    NE = 1,
    GE = 10,
    LT = 11,
    GT = 12,
    LE = 13,
    AL = 15
};

enum cspr_flags {
    overflow_flag = 1, /* 0001 */
    carry_flag = 2, /* 0010 */
    zero_flag = 4,/* 0100 */
    negative_flag = 8 /* 1000 */
};

enum instruction_opcodes {
    AND = 0, /* 0000 */
    EOR = 1, /*0001 */
    SUB = 2, /*0010 */
    RSB = 3, /* 0011 */
    ADD = 4, /* 0100 */
    TEQ = 8, /* 1000 */
    CMP = 10, /* 1010 */
    ORR = 12, /* 1100 */
    MOV = 13 /* 1101 */
};

#endif


