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
 *
 */


bool read_file(machineState *state, char *filename) {
    FILE *binFile;
    binFile = fopen(filename, "rb");
    if (!binFile) {
        fprintf(stderr, "File does not exist. Exiting...\n");
        exit(EXIT_FAILURE); /* non-zero val -- couldn't read file */
    }
    /* Elements to be read are each 4 bytes. Binary files can be any size and
    fread will only read till the end of the file or until the size is met.
    Fread also returns the number of elements read.
    */
     unsigned long fileSize = fread(state->memory, MEMORY_SIZE,1, binFile);
    if (ferror(binFile)) {
        fprintf(stderr, "Error while reading file. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    printf("File successfully read. File size: %lu", fileSize);
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
        fprintf(stderr, "Address specified is too high. Segmentation fault detected.");
        return false;
    }
    state->memory[address] = value;
    return true;
}

uint32_t get_word(machineState *state, uint32_t address) {
    uint32_t fullWord;
    memcpy(&fullWord, &(state->memory[address]), WORD_SIZE_IN_BYTES);
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
    assert(state);
    for (uint32_t i = 0; i < MEMORY_SIZE; i += 4) {
        if (get_word(state, i) != 0) {
            printf("Memory at 0x%x : 0x%x", i, get_word(state, i));
        }
    }

    print_register_values(state);
}

shiftedRegister operand_shift_register(machineState *state, uint32_t instruction) {
    uint32_t rm = instruction & 0xF;
    uint32_t rmContents = get_register(rm, state);
    uint32_t shiftNum = (instruction >> 7) & 0x1F;
    uint32_t shiftType = (instruction >> 5) & 0x3;
    shiftedRegister result = {0, 0};
    switch (shiftType) {
        case LOGICAL_LEFT:
            result.operand2 = rmContents << shiftNum;
            result.carryBit = (rmContents >> (32 - shiftNum)) & 0x1;
            return result;
        case LOGICAL_RIGHT:
            result.operand2 = rmContents >> shiftNum;
            result.carryBit = (rmContents >> (shiftNum - 1)) & 0x1;
            return result;
        case ARITH_RIGHT: {
            uint32_t preservedSign = 0;
            uint32_t signBit = rmContents & 0x80000000;
            for (uint32_t i = 0; i < shiftNum; i++) {
                preservedSign += signBit;
                signBit >>= 1;
            }
            result.operand2 = (rmContents >> shiftNum) + preservedSign;
            result.carryBit = (rmContents >> (shiftNum - 1)) & 0x1;
            return result;
        }
        case ROTATE_RIGHT:
            result.operand2 = (rmContents >> shiftNum) | (rmContents << (32 - shiftNum));
            result.carryBit = (rmContents >> (shiftNum - 1)) & 0x1;
            return result;

        default:
            return result;
    }
}

dataProcessingInstruction decode_dpi(machineState *state, uint32_t instruction) {
    dataProcessingInstruction dpi;
    bool immediate = ((instruction >> 25) & 0x1);
    dpi.opcode = (instruction >> 21) & 0xF;
    dpi.setBit = (instruction >> 20) & 0x1;
    dpi.operand2 = get_register((instruction >> 16) & 0xF, state);
    dpi.rn = (instruction >> 16) & 0xF;
    dpi.rd = (instruction >> 12) & 0xF;
    if (immediate) {
        uint32_t imm = instruction & 0xFF;
        uint32_t rotate = (instruction >> 8) & 0xF;
        dpi.operand2 = imm >> (rotate * 2);
    } else {
        shiftedRegister value = operand_shift_register(state, instruction);
        dpi.operand2 = value.operand2;
        dpi.carryBit = value.carryBit;
    }
    return dpi;
}

static bool is_negative(uint32_t instruction) {
    return (instruction & 1) != 0;
}

multiplyInstruction decode_mi(machineState *state, uint32_t instruction) {
    multiplyInstruction mi;
    mi.rm = instruction & 0xF;
    mi.rs = (instruction >> 8) & 0xF;
    mi.rn = (instruction >> 12) & 0xF;
    mi.rd = (instruction >> 16) & 0xF;
    mi.setBit = (instruction >> 20) & 0x1;
    mi.accumBit = (instruction >> 21) & 0x1;
    return mi;
}

sdtInstruction decode_sdt(machineState *state, uint32_t instruction) {
    sdtInstruction sdti;
    bool offsetBit = (instruction >> 25) & 0x1;
    sdti.indexingBit = (instruction >> 24) & 0x1;
    sdti.upBit = (instruction >> 23) & 0x1;
    sdti.loadBit = (instruction >> 20) & 0x1;
    sdti.rn = (instruction >> 16) & 0xF;
    sdti.rd = (instruction >> 12) & 0xF;

    if (!offsetBit) {
        // 0xFFF represents a mask of the least significant 12 bits
        sdti.offset = instruction & 0xFFF;
    } else {
        sdti.offset = operand_shift_register(state, instruction).operand2;
    }
    return sdti;
}

branchInstruction decode_bi(machineState *state, uint32_t instruction) {
    branchInstruction bi;
    uint32_t offset = instruction & 0xFFFFFF;
    offset <<= 2;
    if (is_negative(instruction >> 23)) {
        offset |= SE_32;
    }
    bi.offset = offset;
    return bi;
}

void decode(machineState *state, uint32_t instruction) {
    decodedInstruction instr;
    instr.condCode = (instruction >> 28) & 0xF;
    if ((instruction >> 26) & 0x1) {
        instr.type = SINGLE_DATA_TRANSFER;
        instr.sdti = decode_sdt(state, instruction);
    } else if ((instruction >> 27) & 0x1) {
        instr.type = BRANCH;
        instr.bi = decode_bi(state, instruction);
    } else if (!((instruction >> 22) & 0x3F) && (((instruction >> 4) & 0xF) == 9)) {
        instr.type = MULTIPLY;
        instr.mi = decode_mi(state, instruction);
    } else {
        if (instruction) {
            instr.type = DATA_PROCESSING;
        } else {
            instr.type = ZERO;
        }
        instr.dpi = decode_dpi(state, instruction);
    }
    uint32_t instrNum = (get_register(PC_REG, state) - 4) / 4;
    state->instructionToDecode[instrNum] = instr;
}

void execute_dpi(machineState *state, dataProcessingInstruction dpi) {
    uint32_t result;
    uint32_t operand1 = get_register(dpi.rn, state);
    switch (dpi.opcode) {
        case AND:
            result = operand1 & dpi.operand2;
            set_register(dpi.rd, state, result);
            break;
        case EOR:
            result = operand1 ^ dpi.operand2;
            set_register(dpi.rd, state, result);
            break;
        case SUB:
            result = operand1 - dpi.operand2;
            set_register(dpi.rd, state, result);
            break;
        case RSB:
            result = dpi.operand2 - operand1;
            set_register(dpi.rd, state, result);
            break;
        case ADD:
            result = operand1 + dpi.operand2;
            set_register(dpi.rd, state, result);
            break;
        case TST:
            result = operand1 & dpi.operand2;
            break;
        case TEQ:
            result = operand1 ^ dpi.operand2;
            break;
        case CMP:
            result = operand1 - dpi.operand2;
            break;
        case ORR:
            result = operand1 | dpi.operand2;
            set_register(dpi.rd, state, result);
            break;
        case MOV:
            result = dpi.operand2;
            break;
        default:;
    }
    if (dpi.setBit) {
        uint32_t zBit = 0;
        if (result == 0) {
            zBit = (1 << 30);
        }
        uint32_t nBit = result & 0x80000000;
        // need to complete implemntation for carry for arithmetic ops
        uint32_t cBit = dpi.carryBit << 29;
        uint32_t flags = nBit + zBit + cBit;
        uint32_t value = get_register(CPSR_REG, state);
        uint32_t mask = 0xFFFFFFF;
        set_register(CPSR_REG, state, (value & mask) + flags);
    }
}

void execute_mi(machineState *state, multiplyInstruction mi) {
    uint32_t res;
    uint32_t acc;
    uint32_t currentCpsr;
    uint32_t lastBit;

    /*Performing the operation */
    acc = (mi.accumBit) ? state->registers[mi.rn] : 0;
    res = (state->registers[mi.rm] * state->registers[mi.rs]) + acc;
    state->registers[mi.rd] = res;

    /*Changing flag status if necessary */
    if (mi.setBit) {
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

void execute_sdti(machineState *state, sdtInstruction sdti) {
    uint32_t baseRegVal = get_register(sdti.rn, state);
    uint32_t srcDestRegVal = get_register(sdti.rd, state);
    if (sdti.loadBit) {
        set_register(sdti.rd, state, baseRegVal);
    } else {
        set_memory(baseRegVal, state, srcDestRegVal);
    }

    if (!sdti.indexingBit) {
        if (sdti.upBit) {
            set_register(sdti.rn, state, baseRegVal + sdti.offset);
        } else {
            set_register(sdti.rn, state, baseRegVal - sdti.offset);
        }
    }
}

// incomplete, to finish
void clear_pipeline(machineState *state) { // to finish
    assert(state);
    state->fetched = -1;
    state->instructionToDecode = NULL;
    state->instructionToExecute = NULL;
    state->fetched_instr = false;
}

void execute_bi(machineState *state) { //branch instr to finish
    state->registers[PC_REG] += state->instructionToExecute->bi.offset;
    clear_pipeline(state);
}

bool check_cond(machineState *state, uint8_t instrCond) {
    // takes the highest 4 bits of the CPSR register (i.e. the cond flags)
    uint32_t cpsrFlags = get_register(CPSR_REG, state) >> SHIFT_COND;
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
            return !(!(cpsrFlags & ZERO_FLAG) &&
                     ((cpsrFlags & NEGATIVE_FLAG) == ((cpsrFlags & NEGATIVE_FLAG) >> 3)));
            // = !GT
        default:
            fprintf(stderr, "An unsupported instruction has been found at PC: %x", get_register(PC_REG, state));
            print_system_state(state);
            exit(EXIT_FAILURE);
    }
}

void execute_instructions(machineState *state) {
    int execNum = (get_register(PC_REG, state) - 8) / 4;
    decodedInstruction decoded = state->instructionToExecute[execNum];
    if (!check_cond(state, decoded.condCode)) {
        // need to check that will exit function at this point
        return;
    }
    switch (decoded.type) {
        case DATA_PROCESSING:
            execute_dpi(state, decoded.dpi);
            break;
        case MULTIPLY:
            execute_mi(state, decoded.mi);
            break;
        case SINGLE_DATA_TRANSFER:
            execute_sdti(state, decoded.sdti);
            break;
        case BRANCH:
            execute_bi(state);
            break;
        case ZERO:
            printf("A zero instruction has been found at PC: 0x%x and the program will terminate.",
                   get_register(PC_REG, state));
            print_system_state(state);
            free(state->memory);
            free(state->registers);
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr,
                    "An unknown instruction type has been found at PC: 0x%x and the program will terminate.",
                    get_register(PC_REG, state));
    }
}

void fetch(machineState *state) {
    state->fetched = get_word(state, get_register(PC_REG, state));
    state->fetched_instr = true;
}

bool decoded_instruction_present(machineState *state) {
    return state->instructionToDecode->type == DATA_PROCESSING
           || state->instructionToDecode->type == SINGLE_DATA_TRANSFER
           || state->instructionToDecode->type == MULTIPLY
           || state->instructionToDecode->type == BRANCH
           || state->instructionToDecode->type == ZERO;
}

void pipeline(machineState *state) {
    while (state->instructionToDecode->type != ZERO) {
        // Execution of decoded instruction.
        if (decoded_instruction_present(state)) {
            execute_instructions(state);
        }

        if (state->fetched_instr) {
            decode(state, state->fetched);
        }

        if (state->instructionToDecode->type != ZERO) {
            fetch(state);
        } else {
            state->fetched_instr = false;
        }

        set_register(PC_REG, state, get_register(PC_REG, state) + 4);
    }

    printf("Congratulations -- the program completed! Printing final system state...\n");
    print_system_state(state);
    printf("\n Terminating emulator now...");

    free(state->instructionToDecode);
    free(state->instructionToExecute);
    free(state->registers);
    free(state->memory);
    exit(EXIT_SUCCESS);
}

const machineState default_state = {
        .registers = {0},
        .memory = {0},
        .fetched_instr = false,
        .instructionToExecute = NULL,
        .fetched = 0,
        .instructionToDecode = NULL,
};

int main(int argc, char **argv) {
   for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    if (argc != 2) {
        fprintf(stderr, "You have not started the program with the correct number of inputs.");
        return EXIT_FAILURE;
    }
    machineState *state = (machineState *) calloc(1, sizeof(machineState));
    *state = default_state;
    read_file(state, argv[1]);
    pipeline(state);


}
