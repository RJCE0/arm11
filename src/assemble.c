#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

typedef struct {
    char **labels;
    uint32_t *labelNextInstr;
} InputFileData;


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
            *(fileData->labelNextInstr + (labelCount - 1)) = 32 * line;
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
                char rn[20];
                char rd[20];
                char operand2[20];
                // rn always register
                fscaf(myfile, "%s", rn);
                fscaf(myfile, "%s", rn);

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

void create_data_processing(bool immediateBit, int opcode, int rnNum, int rdNum, int op2Num) {

}

void create_multiply(bool accBit, bool setCondBit, int rdNum, int rnNum, int rsNum, int rmNum) {

}

void create_single_data_transfer(bool immediateBit, bool prePostIndBit, bool upBit,) {

}

void create_branch() {

}

int main() {
    InputFileData fileData;
    fileData.labels = (char **) malloc (10 * sizeof(char *));
    fileData.labelNextInstr = (int *) malloc (10 * sizeof(int));

    read_file_first(&fileData, "example.txt");
    read_file_second(&fileData, "example.txt");

    printf("\n\nLabel is: %s\n", *(fileData.labels));
    printf("Label instruction points to: %d\n\n", *(fileData.labelNextInstr));

    return 0;
}
