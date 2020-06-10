#include "assemble_utils.h"

// [Function_A, Function_B, ...]
// [0x00012, 0x00000128]

/*
1:  add ... ... ...                     1. mov r2,#23
2:  sub .... ..                         2. wait:
3:  mov ...                             3. sub r2,r2,#1
4:  Loop:                               4. cmp r2,#0
5:  add _ _                             5. bne wait
6:  mov,
7:  beq 0x000017
8:  nd loop:
9:  mov
10: add
<Loop, 17> <End loop, 20>

*/


/* (RJ) branch bit explained for myself:
so for branching, when i see a branch condition it will be made up of two components
the type of branch and a target address. The target address might be an actual address
in or it might be a label, so I'll just use my function I built.
*/

/*  This function does the first pass in two-pass method. It will take a file
name and search it for labels only. It will then save each label name in one
array in parallel with the "next instruction" in another array (all in a struct)

Explained:
the instructions start from 0 so a label on line 3, would effectively be
pointing to instruction 3 (as the label doesn't count) hence instructions of 32
bits would just mean multiplying the number by 4 (bytes), e.g. 3 * 4 = 0x00012.
*/

uint32_t data_processing(instruction *instr) {
    uint32_t condCode = 14 << SHIFT_COND; //shift_cond
    uint32_t immediate = 1 << 25;
    uint32_t opcode = instr->u.opcode << 21;
    uint32_t setBit = 0;
    uint32_t rn = 0;
    uint32_t rd = 0;
    uint32_t operand2 = get_immediate(instr->args[2]);
    if (instr->u.opcode == 14) {
        rd = get_register_num(instr->args[0]) << 12;
    } else if ((instr->u.opcode <= 10) && (instr->u.opcode >= 8)) {
        rn = get_register_num(instr->args[0]) << 16;
        setBit = 1 << 20;
    } else {
        rd = get_register_num(instr->args[0]) << 12;
        rn = get_register_num(instr->args[1]) << 16;
    }
    return condCode | immediate | opcode | setBit | rn | rd | operand2;
}

uint32_t multiply(instruction *instr) {
    uint32_t condition_code = MULTIPLY_CONDITION_CODE << SHIFT_COND;
    uint32_t accBit = instr->u.accBit << 21;
    uint32_t rd = get_register_num(instr->args[0]) << 16;
    uint32_t rn = (instr->u.accBit) ? get_register_num(instr->args[3]) << 12 : 0;
    uint32_t rs = get_register_num(instr->args[1]) << 8;
    uint32_t constRm = (9 << 4) || get_register_num(instr->args[2]);
    return condition_code || accBit || rd || rn || rs || constRm;
}


uint32_t single_data_transfer(instruction *instr) {
    return 0;
}

uint32_t branch(instruction *instr) {
    int32_t offset;
    bool checkLabel = false;
    uint32_t newAddress = get_label_address(instr->state, instr->args[0], &checkLabel);
    if (checkLabel) {
        offset = newAddress - instr->state->pc - 8;
    } else {
        offset = hex_to_decimal(instr->args[0]) - instr->state->pc - 8;
    }
    offset >>= 2;
    return create_branch(instr->u.condCode, offset);
}

uint32_t logical_left_shift(instruction *instr) {
    uint32_t condCode = 14 << SHIFT_COND; //shift_cond
    uint32_t opcode = MOV << 21;
    uint32_t rn = get_register_num(instr->args[0]);
    uint32_t shiftNum = (get_immediate(instr->args[1]) & 0x1F) << 7;
    return condCode || opcode || (rn << 16) || shiftNum || rn;
}

uint32_t halt(instruction *instr) {
    return 0;
}

void read_file_first(firstFile *firstRead, char *inputFileName) {
    FILE *myfile;
    myfile = fopen(inputFileName, "r");
    if (myfile == NULL) {
        printf("file not found, exiting...\n");
        return;
    }
    int line = 0;
    int labelCount = 0;
    while (!feof(myfile)) {
        // need to change string length
        char str[20];
        fscanf(myfile, "%s", str);
        printf("\n---%s---", str);
        if (str[strlen(str) - 1] == ':') {
            labelCount++;
            printf("\nLabel \"%s\" spotted on line: %d\n", str, line + 1);
            printf("So label points to instruction number: %d\n\n", 32 * line);
            firstRead->labels[labelCount - 1] = str;
            firstRead->labelNextInstr[labelCount - 1] = 4 * (line + 1);
        }
        printf("\n---%s---", str);
        //segmentation error here
        do {
            if (feof(myfile)) {
                return;
            }
            printf("check");
        } while (fgetc(myfile) != '\n');
        line++;
    }
    fclose(myfile);
    firstRead->lines = line - labelCount;
    firstRead->numLabels = labelCount;
}

void split_on_commas(char *input, instruction *instr) {
    int count = 0;
    char *pch = strtok(input, ",");
    instr->args[count] = pch;
    while (pch != NULL) {
        pch = strtok(NULL, ",");
        count++;
        instr->args[count] = pch;
    }
}

uint32_t *read_file_second(firstFile *firstRead, char *inputFileName) {
    FILE *myfile;
    myfile = fopen(inputFileName, "r");
    if (myfile == NULL) {
        printf("file not found, exiting...\n");
        return NULL;
    }
    instruction *instr = (instruction *) malloc(sizeof(instruction));
    instr->args = (char **) malloc(5 * sizeof(char *));
    // need counter in first read
    instr->state = (firstFile *) malloc(sizeof(firstFile));
    instr->state = firstRead;
    instr->state->pc = 0;
    uint32_t *decoded = (uint32_t *) malloc(firstRead->lines * sizeof(uint32_t));
    while (!feof(myfile)) {
        char str[20];
        char argsInInstruction[500];
        fscanf(myfile, "%s", str);
        fscanf(myfile, "%s", argsInInstruction);
        split_on_commas(argsInInstruction, instr);
        typedef uint32_t (*func[NUM_INSTRUCTION])(instruction *instr);
        func funcPointers = {data_processing, multiply, single_data_transfer, branch, logical_left_shift, halt};
        uint32_t result = funcPointers[keyfromstring(str, instr)](instr);
        decoded[instr->state->pc / 4] = result;
        if (!result) {
            break;
        }
        instr->state->pc += 4;
    }
    free(instr->args);
    free(instr->state->labels);
    free(instr->state->labelNextInstr);
    free(instr->state);
    free(instr);
    return decoded;
}

/*
Need a function that will split the arguments, on commas. We will be taking in
the full line on the fscanf but won't be able to differentiate between arguments.
it would be best to store these in a struct so we can access each part of the
arguments separately. Those who won't need 3 arguments, initalise the others to null.
*/


//add r1, r2, #0x39

uint32_t create_single_data_transfer(bool immediateBit, bool prePostIndBit, bool upBit) {
    return 0;
}

uint32_t create_branch(uint8_t condCode, int32_t offset) {
    //assuming big endian
    uint32_t middle = 10 << 24;
    uint32_t left_end = condCode << 28;
    //idk how where im meant to put the value afterwards;
    return (left_end | middle | (offset & 0xFFFFFF));
}

int main(int argc, char **argv) {
    firstFile *firstRead = (firstFile *) malloc(sizeof(firstRead));
    firstRead->labels = (char **) malloc(10 * sizeof(char *));
    firstRead->labelNextInstr = (uint32_t *) malloc(10 * sizeof(uint32_t));
    read_file_first(firstRead, argv[1]);
    uint32_t *decoded = (uint32_t *) malloc(firstRead->lines * sizeof(uint32_t));
    decoded = read_file_second(firstRead, argv[1]);
    FILE *binFile;
    binFile = fopen(argv[2], "rb");
    fwrite(decoded, sizeof(uint32_t), firstRead->lines, binFile);
    fclose(binFile);
    free(firstRead->labels);
    free(firstRead->labelNextInstr);
    free(firstRead);
    free(decoded);
    return 0;
}
