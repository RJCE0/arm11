#include "structs.h"
#include <string.h>
#include <stdio.h>

static shiftDict shiftTable[] = {
   {"lsl", LSL}, {"lsr", LSR}, {"asr", ASR}, {"ror", ROR}
};

static dict mnemonicTable[] = {
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
    {"lsl",   DPI,  MOV},
    {"andeq", HALT, HALT}
};


void convert_lsl(instruction *instr){
    strcpy(instr->args[2], "lsl");
    strcpy(instr->args[3], instr->args[1]);
    strcpy(instr->args[1], instr->args[0]);
    instr->argSize = 4;
}

int mnemonic_key(char *key, instruction *instr) {
    for (int i = 0; i < NUM_OPCODE; ++i) {
        dict *table = &mnemonicTable[i];
        if (strcmp(table->key, key) == 0) {
            switch (table->type) {
                case DPI:
                    if (strcmp(key, "lsl") == 0) {
                        convert_lsl(instr);
                    }
                    instr->u.opcode = (dpiType) table->mnemonic;
                    break;
                case MI:
                    instr->u.accBit = (multiplyType) table->mnemonic;
                    break;
                case SDTI:
                    instr->u.loadBit = (sdtType) table->mnemonic;
                    break;
                case BI:
                    instr->u.condCode = (branchType) table->mnemonic;
                    break;
                case HALT:
                default:
                    break;
            }
            return table->type;
        }
    }
    fprintf(stderr, "Mnemonic is unsupported.\n");
    exit(EXIT_FAILURE);
    return -1;
}

int shift_key(char *key) {
  for (int i = 0; i < 4  ; ++i) {
      shiftDict *table = &shiftTable[i];
      if (strcmp(table->key, key) == 0){
          return table->type;
      }
  }
  fprintf(stderr, "Shift is unsupported.\n");
  exit(EXIT_FAILURE);
  return -1;
}
