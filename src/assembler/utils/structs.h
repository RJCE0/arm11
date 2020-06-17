#include "constants.h"
#include <stdint.h>

#ifndef STRUCTS
#define STRUCTS

typedef struct {
    char *s;
    uint32_t i;
} labelInfo;

typedef struct {
    labelInfo *labels;
    int lines;
	int labelCount;
} firstFile;

typedef struct {
    char *key;
    instructionType type;
    int mnemonic;
} dict;

typedef struct {
    uint32_t *decoded;
    int lastInstr;
    int pc;
    labelInfo *labels;
    int labelCount;
} state;

typedef struct {
    union {
        dpiType opcode;
        multiplyType accBit;
        sdtType loadBit;
        branchType condCode;
    } u;
    char **args;
	int argSize;
} instruction;

typedef struct {
    char *key;
    shiftType type;
} shiftDict;

#endif
