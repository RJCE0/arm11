#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

typedef struct {
    char **labels;
    uint32_t *labelNextInstr;
} InputFileData;


/*
1: add ... ... ...
2: sub .... ..
3: mov ...
Loop:
add _ _
mov
beq 0x000017
End loop:
mov
add
<Loop, 17> <End loop, 20>

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

                break;
            case "bne":

                break;
            case "bge":

                break;
            case "blt":

                break;
            case "bgt":

                break;
            case "ble":

                break;
            case "b":

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

add r1, r2, #0x39

bool is_register(char name[], size_t size) {
    return name[0] == 'r';
}

int get_register_num(char name[], size_t size) {
    // name[0] = '';
    return atoi(name+1);
}

int get_immediate(char name[], size_t size) {
    "#0x24a7"

    if (name[1] == '0' && name[2] == 'x') {
        // convert to decimal somehow
    }
    return atoi(name+1);
}

uint32_t label_to_instruction(char label[], size_t size) {
    //
    return 0;
}

uint32_t create_data_processing(bool immediateBit, int opcode, int rnNum, int rdNum, int op2Num) {

}

uint32_t create_multiply(bool accBit, bool setCondBit, int rdNum, int rnNum, int rsNum, int rmNum) {

}

uint32_t create_single_data_transfer(bool immediateBit, bool prePostIndBit, bool upBit, ...) {

}

uint32_t create_branch(...) {

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
