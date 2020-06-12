#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
// Created by Tyrell Duku on 03/06/2020.
//

#define NUM_INSTRUCTION 6
// including bal
#define NUM_OPCODE 24
#define SHIFT_COND 28

typedef enum {
    LSLA,
    LSR,
    ASR,
    ROR
} shiftType;

typedef struct {
    char *key;
    shiftType type;
} shiftDict;

static shiftDict lookup[] = {
   {"lsl", LSLA}, {"lsr", LSR}, {"asr", ASR}, {"ror", ROR}
};

int shift_key(char *key) {
  for (int i = 0; i < 4  ; ++i) {
      shiftDict *sym = &lookup[i];
      if (strcmp(sym->key, key) == 0){
          return sym->type;
      }
  }
  return 0;
}

typedef enum {
    DPI = 0,
    MI = 1,
    SDTI = 2,
    BI = 3,
    HALT = 4,
    LSL = 5
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
    int lastAddress;
    int pc;
    labelInfo *labels;
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



static dict lookuptable[] = {
        {"add",   DPI,  ADD},
        {"sub",   DPI,  SUB},
        {"rsb",   DPI,  RSB},
        {"and",   DPI,  AND},
        {"eor",   DPI,  EOR},
        {"orr",   DPI,  ORR},
        {"mov",   DPI,  MOV},
        {"tst",   DPI,  TST},
        {"teq",   DPI,  TEQ},
        {"cmp",   DPI,  CMP},
        {"mul",   MI,   MUL},
        {"mla",   MI,   MLA},
        {"ldr",   SDTI, LDR},
        {"str",   SDTI, STR},
        {"beq",   BI,   BEQ},
        {"bne",   BI,   BNE},
        {"bge",   BI,   BGE},
        {"blt",   BI,   BLT},
        {"bgt",   BI,   BGT},
        {"ble",   BI,   BLE},
        {"b",     BI,   B},
        {"bal",   BI,   B},
        {"lsl",   LSL,  LSL},
        {"andeq", HALT, HALT}
};

// for function pointer array
void convert_lsl(instruction *instr){
    strcpy(instr->args[2], "lsl");
    strcpy(instr->args[3], instr->args[1]);
    strcpy(instr->args[1], instr->args[0]);
}

int keyfromstring(char *key, instruction *instr) {
    for (int i = 0; i < NUM_OPCODE; ++i) {
        dict *sym = &lookuptable[i];
        if (strcmp(sym->key, key) == 0) {
            switch (sym->type) {
                case DPI:
                    instr->u.opcode = (dpiType) sym->mnemonic;
                    break;
                case MI:
                    instr->u.accBit = (multiplyType) sym->mnemonic;
                    break;
                case SDTI:
                    instr->u.loadBit = (sdtType) sym->mnemonic;
                    break;
                case BI:
                    instr->u.condCode = (branchType) sym->mnemonic;
                    break;
                case LSL:
                    instr->u.opcode = MOV;
                    convert_lsl(instr);
                    return DPI;
                case HALT:
                default:
                    break;
            }
            return sym->type;
        }
    }
    // if label
    return -1;
}

bool is_reg(const char *str) {
    return *str == 'r';
}

bool check_negative_imm(const char *str){
    return *(str + 1) == '-';
}

bool check_negative_reg(const char *str){
    return *str == '-';
}

int get_reg_num(const char *str) {
    return atoi(str + 1);
}

int32_t hex_to_decimal(const char *hex) {
    return strtol(hex, NULL, 0);
}

int get_immediate(const char *str) {
    //"#0x24a7"
    const char *ptr = str;
    while (*str) {
      if (*str++ == '0' && *str == 'x') {
        return hex_to_decimal(str - 1);
      }
    }
    return strtol(ptr+1, NULL, 10);
}

/*
The target address might be an actual address in or it might be a label.
 For this reason, I can made a function to loop through the
first array to find the position of the label address in the second array
*/

bool get_label_address(state *curr, char *str, uint32_t *address) {
    for (int i = 0; i < 10; i++) {
        if (strcmp(curr->labels[i].s, str) == 0) {
          *address = curr->labels[i].i;
          return true;
        }
    }
    return false;
}
