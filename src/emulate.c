#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulate.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

/*The plan for emulator:
 * 1: Load in the binary file, gives 32-bit words. Check the first four bits and compare with
 * first four bits of CPSR register.
 *
 * 2: If they match, prepare to start decoding, if not move to next instruction.
 *
 * 3: Fetch -> Decode -> Execute cycle until we get all zero instruction.
 *
 * Haev
 */


 bool read_file(machineState *state, char *filename) {
     FILE *binFile;
     binFile = fopen(filename, "rb");
     if (!binFile) {
         fprintf(stderr, "File does not exist. Exiting...\n");
         return EXIT_FAILURE; /* non-zero val -- couldn't read file */
     }
     /* Elements to be read are each 4 bytes. Binary files can be any size and
     fread will only read till the end of the file or until the size is met.
     Fread also returns the number of elements read.
     */
     fread(state->memory, MEMORY_SIZE, 1, binFile);
     fclose(binFile);
     return true;
}

uint32_t get_register(uint32_t regNumber, machineState *state) {
    if (regNumber == 13 || regNumber == 14 || regNumber > 16) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range. Returning NULL.");
        exit(EXIT_FAILURE);
    }
    return state->registers[regNumber];
}

bool set_register(uint32_t regNumber, machineState *state, uint32_t value) {
    if (regNumber == 13 || regNumber == 14 || regNumber < 1 || regNumber > 16) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range.");
        return false;
    }
    state->registers[regNumber] = value;
    return true;
}

uint32_t get_memory(uint32_t address, machineState state) {
    return state.memory[address];
}

bool set_memory(uint32_t address, machineState *state, uint8_t value) {
    // future improvement: allow writing of only part of a value (using some sort of truncation)
    if (address > 65532) {
        fprintf(stderr,"Address specified is too high. Segmentation fault detected.");
        return false;
    }
    state->memory[address] = value;
    return true;
}

uint32_t get_word(machineState *state, uint32_t address) {
    uint32_t fullWord;
    memcpy(&fullWord,&(state->memory[address]), WORD_SIZE_IN_BYTES);
    return fullWord;
    // Might be an issue here...
}

/*static void print_binary_array(uint8_t array[], size_t size) {
    for (int i = 0; i < size; i++) {
        printf("%c", array[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
    }
}
*/

void print_register_values(machineState *state) {
    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
        if (i != 13 && i != 14) {
        printf("Register %i : 0x%x", i, get_register(i, state));
        }
        if (i == PC_REG) {
            printf("Program counter: 0x%x", get_register(PC_REG, state));
        }

        if (i == CPSR_REG) {
            printf("CPSR: 0x%x", get_register(CPSR_REG, state));
        }
    }
}

void print_system_state(machineState *state) {
   for (uint32_t i = 0; i < MEMORY_SIZE; i += 4) {
       if (get_word(state, i) != 0) {
           printf("Memory at 0x%x : 0x%x", i, get_word(state, i));
       }
   }

    print_register_values(state);
}

void decode(machineState *state, uint32_t instruction) {
    if(check_instruction(state, instruction)){
        uint32_t mask = 0xFFFFFFF;
        instruction &= mask;
        if (instruction >> 26 == 1) {
            sdt_instruction(state, instruction);
        } else if (instruction >> 24 == 10) {
            branch_instruction(state, instruction);
        } else if (((instruction >> 22) & ((instruction >> 4) & 0xF)) == 9) {
            multiply_instruction(state, instruction);
        } else if ((instruction >> 26) == 0) {
            data_processing_instruction(state, instruction);
        } else {
            fprintf(stderr, "An unsupported instruction has been found at [PC: %x] and the program can no longer continue. Exiting...\n", state->registers[PC_REG]);
            print_system_state(state);
            free(state->registers);
            free(state->memory);
            exit(EXIT_FAILURE);
        }
    }    
}

shiftedRegister operand_shift_register(machineState *state, uint32_t instruction){
    uint32_t rm = instruction & 0xF;
    uint32_t rm_contents = get_register(rm, state);
    uint32_t shift_num = (instruction >> 7) & 0x1F;
    uint32_t shift_type = (instruction >> 5) & 0x3;
    shiftedRegister result = {0,0};
    switch (shift_type) {
        case LOGICAL_LEFT:
            result.operand2 = rm_contents << shift_num;
            result.carryBit = (rm_contents >> (32 - shift_num)) & 0x1;
            return result;
        case LOGICAL_RIGHT:
            result.operand2 = rm_contents >> shift_num;
            result.carryBit = (rm_contents >> (shift_num - 1)) & 0x1;
            return result;
        case ARITH_RIGHT: {
            uint32_t preservedSign = 0;
            uint32_t signBit = rm_contents & 0x80000000;
            for (uint32_t i = 0; i < shift_num; i++) {
                preservedSign += signBit;
                signBit >>= 1;
            }
            result.operand2 = (rm_contents >> shift_num) + preservedSign;
            result.carryBit = (rm_contents >> (shift_num - 1)) & 0x1;
            return result;
        }    
        case ROTATE_RIGHT:
            result.operand2 = (rm_contents >> shift_num) | (rm_contents << (32 - shift_num));
            result.carryBit = (rm_contents >> (shift_num - 1)) & 0x1;
            return result;

        default:
            return result;
    }
}

void data_processing_instruction(machineState *state, uint32_t instruction) {
    bool immediate = ((instruction >> 25) & 0x1) == 1;
    uint32_t opcode = (instruction >> 21) & 0xF;
    uint32_t condition = ((instruction >> 20) & 0x1) == 1;
    uint32_t operand1 = get_register((instruction >> 16) & 0xF, state);
    uint32_t dest = (instruction >> 12) & 0xF;
    uint32_t operand2;
    uint32_t carryBit;
    if (immediate) {
        uint32_t imm = instruction & 0xFF;
        uint32_t rotate = (instruction >> 8) & 0xF;
        operand2 = imm >> (rotate * 2);
    } else {
        shiftedRegister value = operand_shift_register(state, instruction);
        operand2 = value.operand2;
        carryBit = value.carryBit;
    }
    uint32_t result = 0;
    switch (opcode) {
        case AND:
            result = operand1 & operand2;
            set_register(dest, state, result);
            break;
        case EOR:
            result = operand1 ^ operand2;
            set_register(dest, state, result);
            break;
        case SUB:
            result = operand1 - operand2;
            set_register(dest, state, result);
            break;
        case RSB:
            result = operand2 - operand1;
            set_register(dest, state, result);
            break;
        case ADD:
            result = operand1 + operand2;
            set_register(dest, state, result);
            break;
        case TST:
            result = operand1 & operand2;
            break;
        case TEQ:
            result = operand1 ^ operand2;
            break;
        case CMP:
            result = operand1 - operand2;
            break;
        case ORR:
            result = operand1 | operand2;
            set_register(dest, state, result);
            break;
        case MOV:
            result = operand2;
            break;
        default:
         set_register(dest, state, result);
        //Overflow thing Jaimi was talking about
    }
    if (condition){
        uint32_t zBit = 0;
        if (result == 0) {
            zBit = (1 << 30);
        }
        uint32_t nBit = result & 0x80000000;
        uint32_t cBit = carryBit << 29;
        uint32_t flags = nBit + zBit + cBit;
        uint32_t value = get_register(CPSR_REG, state);
        uint32_t mask = 0xFFFFFFF;
        set_register(CPSR_REG, state, (value & mask) + flags);
    }
}

static bool is_negative(uint32_t instruction) {
    return (instruction & 1) != 0;
}

void multiply_instruction(machineState *state, uint32_t instruction) {
    uint32_t res;
    uint32_t acc;
    uint32_t currentCpsr;
    uint32_t lastBit;

    uint32_t aFlag = (instruction >> 21) & 0x1;
    uint32_t sFlag = (instruction >> 20) & 0x1;
    uint32_t rm = (instruction >> 4) & 0xF;
    uint32_t rs = (instruction >> 12) & 0xF;
    uint32_t rn = (instruction >> 16) & 0xF;
    uint32_t rd = (instruction >> 20) & 0xF;
    /*Performing the operation */
    acc = (aFlag) ? state->registers[rn] : 0;
    res = (state->registers[rm] * state->registers[rs]) + acc;
    state->registers[rd] = res;

    /*Changing flag status if necessary */
    if (sFlag) {
         currentCpsr = state->registers[CPSR_REG] & 0xFFFFFFF;
         lastBit = res &= (0x1 >> 31);
        if (res == 0) {
            /* VCZN */
            currentCpsr |= (ZERO_FLAG << SHIFT_COND);
        }
        if (is_negative(res)) {
        currentCpsr |= (NEGATIVE_FLAG << SHIFT_COND);
        }
        state->registers[CPSR_REG] = currentCpsr;
    }
}


void sdt_instruction(machineState *state, uint32_t instruction) {
    bool offsetBit = (((instruction >> 25) & 1) == 1);
    bool preindexingBit = (((instruction >> 24) & 1) == 1);
    bool upBit = (((instruction >> 23) & 1) == 1);
    bool loadBit = (((instruction >> 20) & 1) == 1);
    bool baseRegNum = (((instruction >> 16) & 15) == 1);
    bool srcDestRegNum = (((instruction >> 12) & 15) == 1);
    uint32_t offset;

    if (!offsetBit) {
        // 0xFFF represents a mask of the least significant 12 bits
        offset = instruction & 0xFFF;
    } else {
        offset = operand_shift_register(state, instruction).operand2;
    }

    uint32_t baseRegVal = get_register(baseRegNum, state);
    uint32_t srcDestRegVal = get_register(srcDestRegNum,state);

    if (loadBit) {
        set_register(srcDestRegNum, state, baseRegVal);
    } else {
        set_memory(baseRegVal, state, srcDestRegVal);
    }

    if (!preindexingBit) {
        if (upBit) {
            set_register(baseRegNum, state, baseRegVal + offset);
        } else {
            set_register(baseRegNum, state, baseRegVal - offset);
        }
    }
}

void branch_instruction(machineState *state, uint32_t instruction) {
    uint32_t offset = instruction & 0xFFFFFF;
    offset <<= 2;

    if (is_negative(instruction >> 23)) {
        offset |= SE_32;
    }
    state->registers[PC_REG] += offset;
}

bool check_instruction(machineState *state, uint32_t instruction) {
    // takes highest 4 bits of instruction
    uint32_t instrCond = instruction >> SHIFT_COND;
    // takes the highest 4 bits of the CPSR register (i.e. the cond flags)
    uint32_t cpsrFlags = get_register(CPSR_REG,state) >> SHIFT_COND;
    switch (instrCond) {
        // CSPR FLAGS : VCZN in C
        case AL:
            return true;
        case EQ:
            return cpsrFlags & ZERO_FLAG;
        case NE:
            return !(cpsrFlags & ZERO_FLAG);
        case GE:
            return (cpsrFlags & NEGATIVE_FLAG) == ((cpsrFlags & NEGATIVE_FLAG) >> 3);
        case LT:
            return (cpsrFlags & NEGATIVE_FLAG) != ((cpsrFlags & NEGATIVE_FLAG) >> 3);
        case GT:
            return !(cpsrFlags & ZERO_FLAG) && ((cpsrFlags & NEGATIVE_FLAG) == ((cpsrFlags & NEGATIVE_FLAG) >> 3));
        case LE:
            return !(!(cpsrFlags & ZERO_FLAG) && ((cpsrFlags & NEGATIVE_FLAG) == ((cpsrFlags & NEGATIVE_FLAG) >> 3)));
            // = !GT
        default:
            fprintf(stderr, "An unsupported instruction has been found at PC: %x", get_register(PC_REG, state));
            print_system_state(state);
            exit(EXIT_FAILURE);
    }
}

void execute_instructions(machineState *state) {
    uint32_t current_instruction = get_word(state, get_register(PC_REG, state));
    while (current_instruction != 0) {
        decode(state, current_instruction);
        state->registers[PC_REG] += WORD_SIZE_IN_BYTES;
    }
}

void pipeline(machineState *state) {

}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "You have not started the program with the correct number of inputs.");
        return EXIT_FAILURE;
    }
    machineState *state = (machineState *) calloc(1, sizeof(machineState));
    read_file(state, argv[1]);
    execute_instructions(state);
    print_system_state(state);
    free(state->memory);
    free(state->registers);
    exit(EXIT_SUCCESS);
}
