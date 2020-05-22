#include <stdlib.h>
#include "stdio.h"
#include "stdbool.h"
#include "constants.h"

bool read_file(char **dst, char *filename) {
    FILE *bin_file;
    bin_file = fopen(filename, "rb");
    if (bin_file == NULL) {
        printf("File does not exist. Exiting...\n");
        return EXIT_FAILURE; /* non-zero val -- couldn't read file */
    }
    fread(dst, 1, NUM_ADDRESSES, bin_file);
    return 0;
}

void printBinaryArray(char **array) {
    for (int i = 0; i < 20; i++) {
        printf("%x", array[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
}

int main(int argc, char **argv) {
    char *array[20];
    if (argc != 2) {
        printf("You have not started the program with the correct number of inputs.");
        return EXIT_FAILURE;
    }

    read_file(array, argv[1]);
    printBinaryArray(array);
    return 0;
}
