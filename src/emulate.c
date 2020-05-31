#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulate.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

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
    fread(state->memory, MEMORY_SIZE, 1, binFile);
    if (ferror(binFile)) {
        fprintf(stderr, "Error while reading file. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    fclose(binFile);
    return true;
}

uint32_t get_register(uint32_t regNumber, machineState *state) {
    if (regNumber == 13 || regNumber == 14 || regNumber > 16) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range. Returning NULL.");
        print_system_state(state);
        exit(EXIT_FAILURE);
    }
    return state->registers[regNumber];
}

bool set_register(uint32_t regNumber, machineState *state, uint32_t value) {
    if (regNumber == 13 || regNumber == 14 || regNumber < 0 || regNumber > 16) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range.");
        return false;
    }
    state->registers[regNumber] = value;
    return true;
}

uint32_t get_word(machineState *state, uint32_t address) {
    uint32_t fullWord = 0;
    if (address > MEMORY_SIZE - 4) {
        printf("Error: Out of bounds memory access at address 0x%08x\n", address);
        return 0;
    }
    // converts from little endian to big endian
    for (size_t i = 0; i < 4; i++) {
        fullWord += state->memory[address + i] << (8 * i);
    }
    return fullWord;

}

bool set_word(machineState *state, uint32_t address, uint32_t value) {
    if (address > MEMORY_SIZE - WORD_SIZE_IN_BYTES) {
        fprintf(stderr, "Address specified is too high. Segmentation fault detected.\n");
        return false;
    }
    // converts from big endian to little endian
    for (int i = 0; i < 4; i++) {
        state->memory[address + i] = (value & 0xFF);
        value >>= 8;
    }
    return true;
}

void print_register_values(machineState *state) {
    for (int i = 0; i < NUM_OF_REGISTERS - 4; i++) {
            printf("$%-3d: %10d (0x%08x)\n", i, get_register(i, state), get_register(i, state));
    }
    printf("PC  : %10d (0x%08x)\n",get_register(PC_REG, state), get_register(PC_REG, state));
    printf("CPSR: %10d (0x%08x)\n", get_register(CPSR_REG, state),get_register(CPSR_REG, state));
}

void print_memory(machineState *state, uint32_t address) {
    printf("0x%08x: 0x%02x%02x%02x%02x\n",
    address,
    state->memory[address],
    state->memory[address + 1],
    state->memory[address + 2],
    state->memory[address + 3]);
}

void print_system_state(machineState *state) {
    assert(state);
    printf("Registers:\n");
    print_register_values(state);
    printf("Non-zero memory:\n");
    for (uint32_t i = 0; i < MEMORY_SIZE; i += WORD_SIZE_IN_BYTES) {
        if (get_word(state, i) != 0) {
            print_memory(state, i);
        }
    }
}

void exit_error(machineState *state) {
    print_system_state(state);
    free(state);
    exit(EXIT_FAILURE);
}

static shifted rotate_right(uint32_t contents, uint32_t shiftNum){
    shifted result;
    result.operand2 = (contents >> shiftNum) | (contents << (32 - shiftNum));
    result.carryBit = (contents >> (shiftNum - 1)) & 0x1;
    return result;
}

shifted operand_shift_register(machineState *state, uint16_t instruction) {
    uint32_t rm = instruction & 0xF;
    uint32_t rmContents = get_register(rm, state);
    uint32_t shiftType = (instruction >> 5) & 0x3;
    uint32_t shiftNum;
    if ((instruction >> 4) & 0x1) {
        shiftNum = get_register((instruction >> 8) & 0xF, state) & 0xF;
    } else {
        shiftNum = (instruction >> 7) & 0x1F;
    }
    shifted result = {0, 0};
    if (!shiftNum){
        result.operand2 = rmContents;
        result.carryBit = 0;
        return result;
    }
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
            uint32_t signBit = rmContents & 0x80000000;
            uint32_t preservedSign = 0;
            for (uint32_t i = 0; i < shiftNum; i++) {
                preservedSign += signBit;
                signBit >>= 1;
            }
            result.operand2 = (rmContents >> shiftNum) + preservedSign;
            result.carryBit = (rmContents >> (shiftNum - 1)) & 0x1;
            return result;
        }
        case ROTATE_RIGHT:
            return rotate_right(rmContents, shiftNum);
        default:
            return result;
    }
}

dataProcessingInstruction decode_dpi(machineState *state, uint32_t instruction) {
    dataProcessingInstruction dpi;
    dpi.immediate = ((instruction >> 25) & 0x1);
    dpi.opcode = (instruction >> 21) & 0xF;
    dpi.setBit = (instruction >> 20) & 0x1;
    dpi.rn = (instruction >> 16) & 0xF;
    dpi.rd = (instruction >> 12) & 0xF;
    dpi.operand2 = instruction & 0xFFF;
    return dpi;
}

static bool is_negative(uint32_t instruction, int significant) {
    return (instruction >> significant) & 0x1;
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
    sdti.immediate = (instruction >> 25) & 0x1;
    sdti.indexingBit = (instruction >> 24) & 0x1;
    sdti.upBit = (instruction >> 23) & 0x1;
    sdti.loadBit = (instruction >> 20) & 0x1;
    sdti.rn = (instruction >> 16) & 0xF;
    sdti.rd = (instruction >> 12) & 0xF;
    sdti.offset = instruction & 0xFFF;
    return sdti;
}

branchInstruction decode_bi(machineState *state, uint32_t instruction) {
    branchInstruction bi;
    uint32_t offset = instruction & 0xFFFFFF;
    if (is_negative(offset, 23)) {
        offset |= SE_32;
    }
    offset <<= 2;
    bi.offset = offset;
    return bi;
}

void decode(machineState *state, uint32_t instruction) {
    state->instructionAfterDecode->condCode = (instruction >> 28) & 0xF;
    if (((instruction >> 26) & 0x3) == 0x1 && !((instruction >> 21) & 0x3)) { // NONZERO = TRUE, ZERO = FALSE
        state->instructionAfterDecode->type = SINGLE_DATA_TRANSFER;
        state->instructionAfterDecode->u.sdti = decode_sdt(state, instruction);
    } else if (((instruction >> 24) & 0xF) == 0xA) {
        state->instructionAfterDecode->type = BRANCH;
        state->instructionAfterDecode->u.bi = decode_bi(state, instruction);
    } else if (!((instruction >> 22) & 0x3F) && (((instruction >> 4) & 0xF) == 9)) {
        state->instructionAfterDecode->type = MULTIPLY;
        state->instructionAfterDecode->u.mi = decode_mi(state, instruction);
    } else if (!((instruction >> 26) & 0x3)) {
        if (instruction) {
            state->instructionAfterDecode->type = DATA_PROCESSING;
        } else {
            state->instructionAfterDecode->type = ZERO;
        }
        state->instructionAfterDecode->u.dpi = decode_dpi(state, instruction);
    } else {
        printf("Unsupported instruction type to decode at PC: 0x%08x\n", get_register(PC_REG, state) - 4);
        exit_error(state);
    }
    state->decodedInstr = true;
}


void execute_dpi(machineState *state, dataProcessingInstruction dpi) {
    uint32_t operand2 = 0;
    uint32_t carryBit = 0;
    if (dpi.immediate) {
        uint32_t imm = dpi.operand2 & 0xFF;
        uint32_t rotate = ((dpi.operand2 >> 8) & 0xF) * 2;
        shifted value = rotate_right(imm, rotate);
        operand2 = value.operand2;
        carryBit = value.carryBit;
    } else {
        shifted value = operand_shift_register(state, dpi.operand2);
        operand2 = value.operand2;
        carryBit = value.carryBit;
    }
    uint32_t result;
    uint32_t operand1 = get_register(dpi.rn, state);
    switch (dpi.opcode) {
        case AND:
            result = operand1 & operand2;
            set_register(dpi.rd, state, result);
            break;
        case EOR:
            result = operand1 ^ operand2;
            set_register(dpi.rd, state, result);
            break;
        case SUB:
            carryBit = operand2 <= operand1;
            result = operand1 - operand2;
            set_register(dpi.rd, state, result);
            break;
        case RSB:
            carryBit = operand1 <= operand2;
            result = operand2 - operand1;
            set_register(dpi.rd, state, result);
            break;
        case ADD:
            carryBit = (0xFFFFFFFF - operand1) < operand2;
            result = operand1 + operand2;
            set_register(dpi.rd, state, result);
            break;
        case TST:
            result = operand1 & operand2;
            break;
        case TEQ:
            result = operand1 ^ operand2;
            break;
        case CMP:
            carryBit = operand2 <= operand1;
            result = operand1 - operand2;
            break;
        case ORR:
            result = operand1 | operand2;
            set_register(dpi.rd, state, result);
            break;
        case MOV:
            result = operand2;
            set_register(dpi.rd, state, result);
            break;
        default:
            fprintf(stderr, "An unknown operand has been found at PC: %0x.\n", get_register(PC_REG, state));
            exit_error(state);
    }
    if (dpi.setBit) {
        uint32_t zBit = 0;
        if (result == 0) {
            zBit = (1 << 30);
        }
        uint32_t nBit = result & 0x80000000;
        uint32_t cBit = carryBit << 29;
        uint32_t flags = nBit + zBit + cBit;
        uint32_t current = get_register(CPSR_REG, state);
        uint32_t mask = 0xFFFFFFF;
        set_register(CPSR_REG, state, (current & mask) + flags);
    }
}

void execute_mi(machineState *state, multiplyInstruction mi) {
    uint32_t res = 0;
    uint32_t acc = 0;
    uint32_t currentCpsr;

    /*Performing the operation */
    acc = (mi.accumBit) ? state->registers[mi.rn] : 0;
    res = (state->registers[mi.rm] * state->registers[mi.rs]) + acc;
    state->registers[mi.rd] = res;

    /*Changing flag status if necessary */
    if (mi.setBit) {
        currentCpsr = state->registers[CPSR_REG] & 0xFFFFFFF;
        if (res == 0) {
            /* VCZN */
            currentCpsr |= (Z_FLAG << SHIFT_COND);
        }
        // get tyrell to check
        if (is_negative(res, 31)) {
            currentCpsr |= (N_FLAG << SHIFT_COND);
        }
        state->registers[CPSR_REG] = currentCpsr;
    }
}

void execute_sdti(machineState *state, sdtInstruction sdti) {
    uint32_t offset;
    if (sdti.immediate) {
        offset = operand_shift_register(state, sdti.offset).operand2;
    } else {
        offset = sdti.offset;
    }
    uint32_t rnContents = get_register(sdti.rn, state);
    uint32_t includingOffset;
    if (sdti.upBit) {
        includingOffset = rnContents + offset;
    } else {
        includingOffset = rnContents - offset;
    }
    if (sdti.indexingBit) {
        if (sdti.loadBit) {
            set_register(sdti.rd, state, get_word(state, includingOffset));
        } else {
            set_word(state, includingOffset, get_register(sdti.rd, state));
        }
    } else {
        if (sdti.loadBit) {
            set_register(sdti.rd, state, get_word(state, rnContents));
        } else {
            set_word(state, rnContents, get_register(sdti.rd, state));
        }
        set_register(sdti.rn, state, includingOffset);
    }
}


static void clear_pipeline(machineState *state) {
    assert(state);
    state->fetchedInstr = false;
    state->decodedInstr = false;
}

void execute_bi(machineState *state, branchInstruction bi) {
    if (is_negative(bi.offset, 31)) {
        // if negative then it converts from 2's complement
        state->registers[PC_REG] -= ~(bi.offset - 1);
    } else {
        state->registers[PC_REG] += bi.offset;
    }
    clear_pipeline(state);
}

bool check_cond(machineState *state, uint8_t instrCond) {
    // takes the highest 4 bits of the CPSR register (i.e. the cond flags)
    uint32_t cpsrFlags = get_register(CPSR_REG, state) >> SHIFT_COND;
    switch (instrCond) {
        // CSPR FLAGS : VCZN in C
        case EQ:
            return cpsrFlags & Z_FLAG;
        case NE:
            return !(cpsrFlags & Z_FLAG);
        case GE:
            return (cpsrFlags & N_FLAG) == (cpsrFlags & V_FLAG);
        case LT:
            return (cpsrFlags & N_FLAG) != (cpsrFlags & V_FLAG);
        case GT:
            return !(cpsrFlags & Z_FLAG) && ((cpsrFlags & N_FLAG) == (cpsrFlags & V_FLAG));
        case LE:
            return (cpsrFlags & Z_FLAG) || ((cpsrFlags & N_FLAG) != (cpsrFlags & V_FLAG));
        case AL:
            return true;
        default:
            // should not reach here unless earlier bug with assigning flags
            fprintf(stderr, "An unsupported instruction (unknown cond. code) has been found at PC: %d (%08x)\n",
                    get_register(PC_REG, state), get_register(PC_REG, state));
            exit_error(state);
            return false;
    }
}

void execute_instructions(machineState *state) {
    // exit when ZERO instruction is reached
    if (state->instructionAfterDecode->type == ZERO) {
        print_system_state(state);
        free(state);
        exit(EXIT_SUCCESS);
    }
    // checks condition flags of instruction with CPSR reg, if function returns false then instruction is ignored and not executed
    if (!check_cond(state, state->instructionAfterDecode->condCode)) {
        return;
    }
    switch (state->instructionAfterDecode->type) {
        case DATA_PROCESSING:
            execute_dpi(state, state->instructionAfterDecode->u.dpi);
            break;
        case MULTIPLY:
            execute_mi(state, state->instructionAfterDecode->u.mi);
            break;
        case SINGLE_DATA_TRANSFER:
            execute_sdti(state, state->instructionAfterDecode->u.sdti);
            break;
        case BRANCH:
            execute_bi(state, state->instructionAfterDecode->u.bi);
            break;
        default:
            // instructions should not reach this stage unless error in fetch/decode
            fprintf(stderr,
                    "An unknown instruction type has been found at PC: 0x%x and the program will terminate.",
                    get_register(PC_REG, state) - 8);
            exit(EXIT_FAILURE);
    }
}

void fetch(machineState *state) {
    state->fetched = get_word(state, get_register(PC_REG, state));
    state->fetchedInstr = true;
}

void pipeline(machineState *state) {
    // will repeat fetch decode execute cycle until ZERO instruction or invalid instruction
    while (true) {
        // first checks whether there has been a decoded instruction in previous cycle before executing
        if (state->decodedInstr) {
            execute_instructions(state);
        }
        // first checks whether there has been a fetched instruction in previous cycle before decoding
        if (state->fetchedInstr) {
            decode(state, state->fetched);
        }
        // fetches next instruction and advances PC by 4
        fetch(state);
        state->registers[PC_REG] += 4;
    }
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "You have not started the program with the correct number of inputs.");
        return EXIT_FAILURE;
    }
    machineState *state = (machineState *) calloc(1, sizeof(machineState));
    state->instructionAfterDecode = (decodedInstruction *) malloc(sizeof(decodedInstruction));
    // reads bin file and stores it into memory in our machine state
    read_file(state, argv[1]);
    pipeline(state);
}
