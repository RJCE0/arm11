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

uint32_t getRegister(int regNumber, state_of_machine state) {
    if (regNumber == 13 || regNumber == 14 || regNumber < 0 || regNumber > 16) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range. Returning NULL.");
        return NULL;
    }
    return state.registers[reg_number];
}

bool setRegister(int regNumber, state_of_machine state, uint32_t value) {
    if (regNumber == 13 || regNumber == 14 || regNumber < 1 || regNumber > ) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range.");
        return false;
    }
    state.registers[regNumber] = value;
}

uint32_t getMemory(uint32_t address, state_of_machine state) {
    return state.memory[address];
}

bool setMemory(uint32_t address, state_of_machine state, uint32_t value)
    // future improvement: allow writing of only part of a value (using some sort of truncation)
    if (address > 65532) {
        fprintf("Address specified is too high. Segmentation fault detected.");
        return false;
    }
    state.memory[address] = value;
    return true;
}

bool readFile(state_of_machine state, char *filename) {
    FILE *bin_file;
    bin_file = fopen(filename, "rb");
    if (bin_file == NULL) {
        fprintf(stderr, "File does not exist. Exiting...\n");
        return EXIT_FAILURE; /* non-zero val -- couldn't read file */
    }
    /* Elements to be read are each 4 bytes. Binary files can be any size and
    fread will only read till the end of the file or until the size is met.
    Fread also returns the number of elements read.
    */
    int elements = fread(state.memory, MEMORY_SIZE / 4, 4, bin_file);

    fclose(bin_file);
    return true;
}

void executeInstructions(state_of_machine state) {
    uint32_t programCounter = getRegister(PC_REG, state);
    while (programCounter != 0) {
        decode(state, programCounter);
        programCounter += 4;
    }
}

bool checkInstruction(struct state_of_machine machine, uint32_t instructionPtr) {
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
        sdtInstruction(machine, instruction);
    } else if (instruction >> 24 == 10) {
        branchInstruction(machine, instruction);
    } else if (((instruction >> 22) | (instruction >> 4)) == 9) {
        multiplyInstruction(machine, instruction);
    } else if ((instruction >> 26) == 0) {
        dataProcessingInstruction(machine, instruction);
    } else {
         fprintf(stderr, "An unsupported instruction has been found at PC: %x", machine.registers[PC_REG]);
         exit(EXIT_FAILURE);
    }
}

static bool isNegative(uint32_t instruction) {
    return (instruction & 1) != 0;
}

void branchInstruction(struct state_of_machine machine, uint32_t instruction) {
    uint32_t offset = instruction & 0xFFFFFF;
    offset <<= 2;

    if (is_negative(instruction >> 23)) {
        offset |= SIGNEXTENSION__TO_32;
    }
    machine.registers[PC_REG] += offset;
}

void multiplyInstruction(struct state_of_machine machine, uint32_t instruction) {
    uint32_t res;
    uint32_t acc;
    uint32_t current_cpsr;
    uint32_t last_bit;

    uint32_t a_flag = (instruction >> 21) & 0x1;
    uint32_t s_flag = (instruction >> 20) & 0x1;
    uint32_t rm = (instruction >> 4) & 0xF;
    uint32_t rs = (instruction >> 12) & 0xF;
    uint32_t rn = (instruction >> 16) & 0xF;
    uint32_t rd = (instruction >> 20) & 0xF;

    acc = (a_flag) ? machine.registers[rn] : 0;
    res = (machine.registers[rm] * machine.registers[rs]) + acc;
    machine.registers[rd] = res;
    current_cpsr = machine.registers[CPSR_REG] & 0xFFFFFFF;
    last_bit = res &= (0x1 >> 31);
    if (s_flag) {
        if (res == 0) {
            /* VCZN */
            current_cpsr |= (zero_flag << SHIFT_COND);
        }
        if (is_negative(res)) {
        current_cpsr |= (negative_flag << SHIFT_COND);
        }
    }

    machine.registers[CPSR_REG] = current_cpsr;
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
        fprintf(stderr, "You have not started the program with the correct
                                                        number of inputs.");
        return EXIT_FAILURE;
    }

    struct state_of_machine state;

    // size of 1 allows memory to be byte addressable
    state.memory = (uint8_t *) calloc (MEMORY_SIZE, 1);
    state.registers = (uint32_t *) calloc (NUM_OF_REGISTERS, sizeof(uint32_t));

    readFile(state, argv[1]);
    executeInstructions(state);
    // printBinaryArray(memory, 100);
    return 0;
}
