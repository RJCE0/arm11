#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
// Created by Tyrell Duku on 03/06/2020.
//

#define NUM_INSTRUCTION 6
// including bal
#define NUM_OPCODE 24
#define SHIFT_COND 28

typedef enum {
    DPI,
    MI,
    SDTI,
    BI,
    LSL,
    HALT
} instructionType;

typedef enum {
    ADD = 4,
    SUB = 2,
    RSB = 3,
    AND = 0,
    EOR = 1,
    ORR = 13,
    MOV = 14,
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
    char *key;
    instructionType type;
    int mnemonic;
} dict;

typedef struct {
    union {
        dpiType opcode;
        multiplyType accBit;
        sdtType loadBit;
        branchType condCode;
    } u;
    char **args;
    state *state;
} instruction;

typedef struct {
    char **labels;
    int pc;
} state;

typedef struct {
    char **labels;
    int lines;
} firstRead

static dict lookuptable[] = {
        { "add", DPI, ADD}, { "sub", DPI, SUB}, { "rsb", DPI, RSB}, { "and", DPI, AND},
        { "eor", DPI, EOR}, { "orr", DPI, ORR}, { "mov", DPI, MOV}, { "tst", DPI, TST},
        { "teq", DPI, TEQ}, { "cmp", DPI, CMP}, { "mul", MI, MUL}, { "mla", MI, MLA},
        { "ldr", SDTI, LDR}, { "str", SDTI, STR}, { "beq", BI, BEQ}, { "bne", BI, BNE},
        { "bge", BI, BGE}, { "blt", BI, BLT}, { "bgt", BI, BGT}, { "ble", BI, BLE},
        { "b", BI, B},  {"bal", BI, B}, { "lsl", LSL, LSL}, {"andeq", HALT, HALT}
};

// for function pointer array
typedef uint32_t (*func[NUM_INSTRUCTION]) (instruction *instr);

int keyfromstring(char *key, instruction *instr){
    for (int i = 0; i < NUM_OPCODE  ; ++i) {
        dict *sym = &lookuptable[i];
        if (strcmp(sym->key, key) == 0){
            switch (sym->type) {
                case DPI:
                    instr->u.opcode = sym->mnemonic;
                    break;
                case MI:
                    instr->u.accBit = sym->mnemonic;
                    break;
                case SDTI:
                    instr->u.loadBit = sym->mnemonic;
                    break;
                case BI:
                    instr->u.condCode = sym->mnemonic;
                    break;
                case LSL:
                case HALT:
                default:
                    break;
            }
            return sym->type;
        }
    }
    //  to change
    fprintf(stderr, "Instruction not supported by assembler\n");
    return 0;
}

bool is_register(char *name) {
    return name[0] == 'r';
}

int get_register_num(char *name) {
    // name[0] = '';
    return atoi(name+1);
}

int get_immediate(char *name) {
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
