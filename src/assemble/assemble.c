
#include "assemble_utils.h"




// [Function_A, Function_B, ...]
// [0x00012, 0x00000128]

/*
1:  add ... ... ...                     1. mov r2,#23
2:  sub .... ..                         2. wait:
3:  mov ...                             3. sub r2,r2,#1
4:  Loop:                               4. cmp r2,#0
5:  add _ _                             5. bne wait
6:  mov
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

void data_processing(instruction *instr){
    uint32_t condCode = 14 << SHIFT_COND; //shift_cond
    uint32_t immediate = 1 << 25;
    uint32_t opcode = instr->u.opcode << 21;
    uint32_t setBit = 0;
    uint32_t rn = 0;
    uint32_t rd = 0;
    uint32_t operand2 = = get_immediate(instr->args[2]);
    if (instr->u.opcode == 14) {
        rd = get_register_num(instr->args[0]) << 12;
    } else if ((inst->u.opcode <= 10) && (inst->u.opcode >= 8)){
        rn = get_register_num(instr->args[0]) << 16;
        setBit = 1 << 20;
    } else {
        rd = get_register_num(instr->args[0]) << 12;
        rn = get_register_num(instr->args[1]) << 16;
    }
    uint32_t result = condCode | immediate | opcode | setBit | rn | rd | operand2;
}

void multiply(instruction *instr){

}

void single_data_transfer(instruction *instr){

}

/*
Few problems:

Had to include the pc counter as being passed to the function

gonna need the labels 2d array in order to find the address of the label

*/

void branch(instruction *instr){
    uint32_t offset;
    uint32_t newAddress = get_label_address(instr->state.labels, instr->args[0]);
    if(newAddress == NULL){
        offset = instr->state.pc - hex_to_decimal(instr->args[0]);
    }
    else{
        offset = instr->state.pc - newAddress;
    }
    offset += 8;
    offset >>= 2;
    create_branch(instr->u.condCode, offset);
}

void logical_left_shift(instruction *instr){

}

void halt(instruction *instr){

}

void read_file_first(inputFileData *fileData, char *inputFileName) {

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
            printf("\nLabel \"%s\" spotted on line: %d\n", str, line+1);
            printf("So label points to instruction number: %d\n\n", 32 * line);
            *(fileData->labels + (labelCount - 1)) = str;
            *(fileData->labelNextInstr + (labelCount - 1)) = 32 * line - labelCount;
        }
        printf("\n---%s---", str);
        do {
            if (feof(myfile)) {
                return;
            }
        } while (fgetc(myfile) != '\n');

        line++;
    }

    fclose(myfile);
}

void split_on_commas(char *input, instruction *instr){
    int count = 0;
    char *pch = strtok(input, ",");
    instr->args = pch;
    while (pch != NULL) {
        pch = strtok(NULL, ",");
        count++;
        instr->args[count] = pch;
    }
}

void read_file_second(inputFileData fileData, char *inputFileName) {
    fileData.pc = 0;
    FILE *myfile;
    myfile = fopen(inputFileName, "r");
    if (myfile == NULL) {
        printf("file not found, exiting...\n");
        return;
    }
    while (!feof(myfile)) {
        char str[20];
        char argsInInstruction[500];
        fscanf(myfile, "%s", str);
        fscanf(myfile, "%s", argsInInstruction);

        char **arrayOfStrs = malloc(5*sizeof(char *));
        instruction *instr = malloc(sizeof(instruction));
        instr->args = arrayOfStrs;
        instr->state = &fileData;
        split_on_commas(argsInInstruction, instr);
        fileData.pc += 4;
        typedef void (*func[NUM_INSTRUCTION]) (instruction *instr);
        func funcPointers = {data_processing, multiply, single_data_transfer, branch, logical_left_shift, halt}
        (*func[keyfromstring(str, instr)]) (instr);
        //After this we know the instruction type
        //so we need a switch so we can go into the void functions

    }
}

/*
Need a function that will split the arguments, on commas. We will be taking in
the full line on the fscanf but won't be able to differentiate between arguments.
it would be best to store these in a struct so we can access each part of the
arguments separately. Those who won't need 3 arguments, initalise the others to null.
*/


//add r1, r2, #0x39




uint32_t create_multiply(bool accBit, bool setCondBit, int rdNum, int rnNum, int rsNum, int rmNum) {
}

uint32_t create_single_data_transfer(bool immediateBit, bool prePostIndBit, bool upBit, ...) {

}

uint32_t create_branch(uint8_t condCode, uint32_t offset) {
    uint8_t middle = 1010;
}

int main() {
    InputFileData fileData;
    fileData.labels = (char **) malloc (10 * sizeof(char *));
    fileData.labelNextInstr = (int *) malloc (10 * sizeof(int));

    read_file_first(&fileData, "example.txt");
    read_file_second(fileData, "example.txt");

    free(fileData.labels);
    free(fileData.labelNextInstr);
    return 0;
}
