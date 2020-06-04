#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
// Created by Tyrell Duku on 03/06/2020.
//

#define NUM_INSTRUCTION 6
#define NUM_OPCODE 23

typedef enum {
    DPI,
    MI,
    SDTI,
    BI,
    LSL,
    HALT
} instructionType;

typedef enum {
    ADD,
    SUB,
    RSB,
    AND,
    EOR,
    ORR,
    MOV,
    TST,
    TEQ,
    CMP,
    MUL,
    MLA,
    LDR,
    STR,
    BEQ,
    BNE,
    BGE,
    BLT,
    BGT,
    BLE,
    B,
} opcodes;

typedef struct {
    char *key;
    int instruction;
    int opcode;
} dict;

typedef struct {
    char **labels;
    uint32_t *labelNextInstr;
    int pc;
} InputFileData;

typedef struct {
    opcodes oc;
    char **args;
} Instruction;

static dict lookuptable[] = {
        { "add", DPI, ADD}, { "sub", DPI, SUB}, { "rsb", DPI, RSB}, { "and", DPI, AND},
        { "eor", DPI, EOR}, { "orr", DPI, ORR}, { "mov", DPI, MOV}, { "tst", DPI, TST},
        { "teq", DPI, TEQ}, { "cmp", DPI, CMP}, { "mul", MI, MUL}, { "mla", MI, MLA},
        { "ldr", SDTI, LDR}, { "str", SDTI, STR}, { "beq", BI, BEQ}, { "bne", BI, BNE},
        { "bge", BI, BGE}, { "blt", BI, BLT}, { "bgt", BI, BGT}, { "ble", BI, BLE},
        { "b", BI, B},  {"bal", BI, B}, { "lsl", LSL, LSL}, {"andeq", HALT, HALT}
};


int keyfromstring(char *key, Instruction *instruction){
    for (int i = 0; i < NUM_OPCODE  ; ++i) {
        dict *sym = &lookuptable[i];
        if (strcmp(sym->key, key) == 0)
            instruction->oc = sym->opcode;
        return sym->instruction;
    }
    //  to change
    fprintf(stderr, "Instruction not supported by assembler\n");
    return 0;
}

bool is_register(char name[], size_t size) {
    return name[0] == 'r';
}

int get_register_num(char name[], size_t size) {
    // name[0] = '';
    return atoi(name+1);
}

int get_immediate(char name[], size_t size) {
    //"#0x24a7"

    if (name[1] == '0' && name[2] == 'x') {
        // convert to decimal somehow
    }
    return atoi(name+1);
}

//think mazen might need this for get immediate (RJ)
// you just pass in "0x245A2175" for example and it gives back the uint32_t
// note, i assumed that the hex was in big endian.
uint32_t hex_to_decimal(char hex[]){
    int len = strlen(hex);
    len--;

    uint32_t dec_val = 0;

    for (int i=2; i<=len; i++) {
        if (hex[i]>='0' && hex[i]<='9') {
            dec_val += (hex[i] - 48) * pow(16, (len-i));
        }

        else if (hex[i]>='A' && hex[i]<='F') {
            dec_val += (hex[i] - 55 ) * pow(16, (len-i));
        }
    }
    return dec_val;
}

uint32_t label_to_instruction(char label[], size_t size) {
    //
    return 0;
}

/*
The target address might be an actual address in or it might be a label.
 For this reason, I can made a function to loop through the
first array to find the position of the label address in the second array
*/
uint32_t get_label_address(char **labelsArray, char *str ){
    int i = 0;
    int cols = sizeof(*labelsArray);
    bool check = i < cols;
    while(check && strcmp(str, *(*labelsArray + i)) != 0){
        i++;
    }
    if (check){
        return *(*(labelsArray + 1) + i);
    }
    else{
        printf("Wasn't a label");
        return NULL;
    }
    // (RJ) need to check this works
}


