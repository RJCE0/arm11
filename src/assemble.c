#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    char **labels;
    uint32_t *labelNextInstr;
} InputFileData;

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

void read_file_second(InputFileData fileData, char *inputFileName) {
    FILE *myfile;
    myfile = fopen(inputFileName, "r");
    if (myfile == NULL) {
        printf("file not found, exiting...\n");
        return;
    }
    while (!feof(myfile)) {
        char str[20];
        fscanf(myfile, "%s", str);
        switch (str) {
            // Data Processing type 1:
            case "add":

                break;
            case "sub":

                break;
            case "rsb":

                break;
            case "and":

                break;
            case "eor":

                break;
            case "orr":

                break;

            // Data Processing type 2:
            case "mov":

                break;

            // Data Processing type 3:
            case "tst":

                break;
            case "teq":

                break;
            case "cmp":

                break;

            // Multiply instructions
            case "mul":

                break;
            case "mla":

                break;

            // Single Data Transfer instructions
            case "ldr":

                break;
            case "str":

                break;

            // Branch instructions
            case "beq":
                uint8_t condCode = 0000;
                //offset = current address - branch address;
                //offset >>= 2;
                // need to take care of sumn about an off-by-8 bytes  
                // effect that will occur due to the ARM pipeline
                break;
            case "bne":
                uint8_t condCode = 0001;
                //offset = current address - branch address;
                //offset >>= 2;
                break;
            case "bge":
                uint8_t condCode = 1010;
                //offset = current address - branch address;
                //offset >>= 2;
                break;
            case "blt":
                uint8_t condCode = 1011;
                //offset = current address - branch address;
                //offset >>= 2;
                break;
            case "bgt":
                uint8_t condCode = 1100;
                //offset = current address - branch address;
                //offset >>= 2;
                break;
            case "ble":
                uint8_t condCode = 1101;
                //offset = current address - branch address;
                //offset >>= 2;
                break;
            case "b":
            case "bal":
                uint8_t condCode = 1110;
                //offset = current address - branch address;
                //offset >>= 2;
                break;

            // Special
            case "lsl":

                break;
            case "andeq":

                break;
            default:

                break;
        }
    }
}

//add r1, r2, #0x39

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
} //think mazen might need this for get immediate (RJ)
// you just pass in "0x245A2175" for example and it gives back the uint32_t
// note, i assumed that the hex was in big endian.

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
    while(check && strcmp(str, *(*labelsArray + i) != 0)){
        i++;
    }
    if (check){
        return *(*(labelsArray + 1) + i);
    }
    else{
       printf("String entered wasn't a label");
    }
    // (RJ) need to check this works
}


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
    read_file_second(&fileData, "example.txt");

    free(fileData.labels);
    free(fileData.labelNextInstr);
    return 0;
}
