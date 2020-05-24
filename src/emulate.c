#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulate.h"
#include <assert.h>


/*The plan for emulator:
 * 1: Load in the binary file, gives 32-bit words. Check the first four bits and compare with
 * first four bits of CPSR register.
 *
 * 2: If they match, prepare to start decoding, if not move to next instruction.
 *
 * 3: Fetch -> Decode -> Execute cycle until we get all zero instruction.
 */


 bool read_file(uint8_t dst[], char *filename) {
    FILE *bin_file;
    bin_file = fopen(filename, "rb");
    if (bin_file == NULL) {
        fprintf(stderr, "File does not exist. Exiting...\n");
        return EXIT_FAILURE; /* non-zero val -- couldn't read file */
    }

    fread(dst,NO_ADDRESSES,1, bin_file);
    fclose(bin_file);
    return true;
}

bool check_instruction(struct state_of_machine machine, uint32_t instruction) {
    instruction >>= SHIFT_COND;
    char cpsr_flags = machine.registers[CPSR_REG] >> SHIFT_COND;
    switch (instruction) {
        // CSPR FLAGS : VCZN in C
        case AL:
            machine.has_instruction = true;
            return true;
        case EQ:
            machine.has_instruction = true;
            return cpsr_flags & zero_flag;
        case NE:
            machine.has_instruction = true;
            return !(cpsr_flags & zero_flag);
        case GE:
            machine.has_instruction = true;
            return (cpsr_flags & negative_flag) == ((cpsr_flags & negative_flag) >> 3);
        case LT:
            machine.has_instruction = true;
            return (cpsr_flags & negative_flag) != ((cpsr_flags & negative_flag) >> 3);
        case GT:
            machine.has_instruction = true;
            return !(cpsr_flags & zero_flag) && ((cpsr_flags & negative_flag) == ((cpsr_flags & negative_flag) >> 3));
        case LE:
            machine.has_instruction = true;
            return !(!(cpsr_flags & zero_flag) && ((cpsr_flags & negative_flag) == ((cpsr_flags & negative_flag) >> 3)));
        default:
            fprintf(stderr, "An unsupported instruction has been found at PC: %x", machine.registers[PC_REG]);
            exit(EXIT_FAILURE);
    }
}

void decode(struct state_of_machine machine, uint32_t instruction) {
    uint32_t mask = 0xFFFFFFF;
    instruction &= mask;
    // Removed condition bits from instruction.
    if (instruction >> 26 == 1) {
        sdt(machine, instruction);
    } else if (instruction >> 24 == 10) {
        branch(machine, instruction);
    } else if (((instruction >> 22) | (instruction >> 4)) == 9) {
        multiply_instruction(machine, instruction);
    } else if ((instruction >> 26) == 0) {
        data_processing(machine, instruction);
    } else {
         fprintf(stderr, "An unsupported instruction has been found at PC: %x", machine.registers[PC_REG]);
         exit(EXIT_FAILURE);
    }
}



static bool is_negative(uint32_t instruction) {
    return (instruction & 1) != 0;
}


void branch(struct state_of_machine machine, uint32_t instruction) {
    int32_t offset = instruction & 0xFFFFFF;
    offset <<= 2;

    if (is_negative(instruction >> 23)) {
        offset |= SIGN_EXTENSION__TO_32;
    }

    machine.registers[PC_REG] += offset;
}

static void printBinaryArray(uint8_t array[], size_t size) {
    for (int i = 0; i < size; i++) {
        printf("%c", array[i]);
        if (i % 32 == 31) {
            printf("\n");
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "You have not started the program with the correct number of inputs.");
        return EXIT_FAILURE;
    }

    uint8_t *memory = (uint8_t *)calloc(8, 1);

    read_file(memory, argv[1]);
    printBinaryArray(memory, 100);
    return 0;
}
