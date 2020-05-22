//
// Created by Tyrell Duku on 21/05/2020.
//

#define WORD_SIZE 32
#define NUM_ADDRESSES 65536
#define NUM_OF_REGISTERS 32
#define CPSR_REG 16
#define PC_REG 16
#define SHIFT_COND 28

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


