
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

void data_processing(Instruction *instruction){

}

void multiply(Instruction *instruction){

}

void single_data_transfer(Instruction *instruction){

}

/* 
Few problems: 

Had to include the pc counter as being passed to the function

gonna need the labels 2d array in order to find the address of the label

*/

void branch(Instruction *instruction, char **labels, int pc){
    uint8_t condCode;
    uint32_t offset;
    uint32_t newAddress = get_label_address(labels, instruction->args[0]);
    switch (instruction->oc){
    case BEQ:
        condCode = 0000;
        break;
    case BNE:
        condCode = 0001;
        break;
    case BGE:
        condCode = 1010;
        break;
    case BLT:
        condCode = 1011;
        break;
    case BGT:
        condCode = 1100;
        break;
    case BLE:
        condCode = 1101;
        break;
    case B:
        condCode = 1110;
        break;
    default:
        printf("Not a valid opcode");
        exit(EXIT_FAILURE);
    }

    if(newAddress == NULL){
        offset = pc - hex_to_decimal(instruction->args[0]);
    }
    else{
        offset = pc - newAddress;
    }
    offset += 8;
    offset >>= 2;
    create_branch(condCode, offset);
}

void logical_left_shift(Instruction *instruction){

}

void halt(Instruction *instruction){

}

void read_file_first(InputFileData *fileData, char *inputFileName) {

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

void split_on_commas(char *input, Instruction *instruction){
    int count = 0;
    char *pch = strtok(input, ",");
    instruction->args = pch;
    while (pch != NULL) {
        pch = strtok(NULL, ",");
        count++;
        instruction->args[count] = pch;
    }
}

void read_file_second(InputFileData fileData, char *inputFileName) {
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
        Instruction *instruction = malloc(sizeof(Instruction));
        split_on_commas(argsInInstruction, instruction);
        fileData.pc += 4;
        void (*func[NUM_INSTRUCTION]) (Instruction *instruction);
        func[0] = data_processing;
        func[1] = multiply;
        func[2] = single_data_transfer;
        func[3] = branch;
        func[4] = logical_left_shift;
        func[5] = halt;

        (*func[keyfromstring(str, instruction)]) (instruction);
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




uint32_t create_data_processing(bool immediateBit, int opcode, int rnNum, int rdNum, int op2Num) {

}

uint32_t create_multiply(bool accBit, bool setCondBit, int rdNum, int rnNum, int rsNum, int rmNum) {
}

uint32_t create_single_data_transfer(bool immediateBit, bool prePostIndBit, bool upBit, ...) {

}

uint32_t create_branch(uint8_t conCode, uint32_t offset) {
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
