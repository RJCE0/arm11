#include <stdlib.h>
#include "stdio.h"
#include "stdbool.h"
#include "constants.h"
#include "state_of_machine.h"


/*The plan for emulator:
 * 1: Load in the binary file, gives 32-bit words. Check the first four bits and compare with
 * first four bits of CPSR register.
 *
 * 2: If they match, prepare to start decoding, if not move to next instruction.
 *
 * 3: Fetch -> Decode -> Execute cycle until we get all zero instruction.
 */


bool read_file(char dst[], char *filename) {
    FILE *bin_file;
    bin_file = fopen(filename, "rb");
    if (bin_file == NULL) {
        fprintf(stderr, "File does not exist. Exiting...\n");
        return EXIT_FAILURE; /* non-zero val -- couldn't read file */
    }
    fread(dst, 1, NUM_ADDRESSES, bin_file);
    return 0;
}

bool check_instruction(struct state_of_machine machine, uint32_t instruction) {
    instruction >>= SHIFT_COND;
    char cpsr_flags = machine.registers[CPSR_REG] >> SHIFT_COND;
    switch (instruction) {
        case EQ:
            return cpsr_flags & zero_flag;
        case NE:
            return !(cpsr_flags & zero_flag);
        case GE:
    }
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
    struct state_of_machine machine = {{0}, {0}};
    if (argc != 2) {
        fprintf(stderr, "You have not started the program with the correct number of inputs.");
        return EXIT_FAILURE;
    }

    read_file(machine.memory, argv[1]);
    return 0;
}
