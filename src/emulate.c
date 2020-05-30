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
    //Needs to be fixed, reads backwards for some reason smh
    fread(state->memory, MEMORY_SIZE, 1, binFile);
    if (ferror(binFile)) {
        fprintf(stderr, "Error while reading file. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    fclose(binFile);
    return true;
}

uint32_t get_memory(machineState *state, uint32_t address) {
    uint32_t result = 0;
    for (int i = 0; i < 4; i++) {
        result += state->memory[address+i];
        if (i == 3) {
            break;
        }
        result <<=8;
    }
    return result;
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
    if (regNumber == 13 || regNumber == 14 || regNumber < 1 || regNumber > 16) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range.");
        return false;
    }
    state->registers[regNumber] = value;
    return true;
}

uint32_t get_word(machineState *state, uint32_t address) {
    uint32_t byte1 = state -> memory[address+3] << 0x18;
    uint32_t byte2 = state->memory[address+2] << 0x10;
    uint32_t byte3 = state->memory[address+1] << 0x8;
    uint32_t byte4 = state->memory[address];
    uint32_t fullWord = byte1 + byte2 + byte3 + byte4;

    return fullWord;
}

bool set_word(machineState *state, uint32_t address, uint32_t value){
    if (address > 65532) {
        fprintf(stderr, "Address specified is too high. Segmentation fault detected.\n");
        return false;
    }
    for (int i = 0; i < 4; i++) {
        state->memory[address + i] = (value & 0xFF);
        value >>= 8;
    }
    return true;
}

void print_register_values(machineState *state) {
    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
        if (i != 13 && i != 14) {
            printf("Register %i : 0x%08x\n", i, get_register(i, state));
        }
        if (i == PC_REG) {
            printf("Program counter: 0x%08x\n", get_register(PC_REG, state));
        }

        if (i == CPSR_REG) {
            printf("CPSR: 0x%08x\n", get_register(CPSR_REG, state));
        }
    }
}

void print_system_state(machineState *state) {
    assert(state);
    for (uint32_t i = 0; i < MEMORY_SIZE; i += 4) {
        if (get_word(state, i) != 0) {
            printf("Memory at 0x%08x : 0x%08x\n", i, get_memory(state,i));
        }
    }

    print_register_values(state);
}

shiftedRegister operand_shift_register(machineState *state, uint16_t instruction) {
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
            result.operand2 = (rmContents >> shiftNum) | (rmContents << (32 - shiftNum));
            result.carryBit = (rmContents >> (shiftNum - 1)) & 0x1;
            return result;
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
    instruction &= 0xFFFFFFF;
    if (((instruction >> 26) & 0x3) == 0x1 && !(instruction >> 21 )) { // NONZERO = TRUE, ZERO = FALSE
        instr.type = SINGLE_DATA_TRANSFER;
        instr.u.sdti = decode_sdt(state, instruction);
    } else if ((instruction >> 24) == 0xA) {
        instr.type = BRANCH;
        instr.u.bi = decode_bi(state, instruction);
    } else if (!((instruction >> 22) & 0x3F) && (((instruction >> 4) & 0xF) == 9)) {
        instr.type = MULTIPLY;
        instr.u.mi = decode_mi(state, instruction);
    } else if (!((instruction >> 26) & 0x3)) {
        if (instruction) {
            instr.type = DATA_PROCESSING;
        } else {
            instr.type = ZERO;
        }
        instr.u.dpi = decode_dpi(state, instruction);
    } else {
        printf("Unsupported instruction type at PC: 0x%08x\n");
        print_system_state(state);
        free(state);
        exit(EXIT_FAILURE);
    }
    state->decodedInstr = true;
    state->instructionAfterDecode = instr;
}




void execute_dpi(machineState *state, dataProcessingInstruction dpi){
    uint32_t op2 = 0;
    uint32_t carryBit = 0;
    if (dpi.immediate) {
        uint32_t imm = dpi.operand2 & 0xFF;
        uint32_t rotate = (dpi.operand2 >> 8) & 0xF;
        op2 = imm >> (rotate * 2);
    } else {
        shiftedRegister value = operand_shift_register(state, dpi.operand2);
        op2 = value.operand2;
        carryBit = value.carryBit;
    }
    uint32_t result;
    uint32_t operand1 = get_register(dpi.rn, state);
    switch (dpi.opcode) {
        case AND:
            result = operand1 & op2;
            set_register(dpi.rd, state, result);
            break;
        case EOR:
            result = operand1 ^ op2;
            set_register(dpi.rd, state, result);
            break;
        case SUB:
            carryBit = op2 <= operand1;
            result = operand1 - op2;
            set_register(dpi.rd, state, result);
            break;
        case RSB:
            carryBit = operand1 <= op2;
            result = op2 - operand1;
            set_register(dpi.rd, state, result);
            break;
        case ADD:
            carryBit = (0xFFFFFFFF - operand1) < op2;
            result = operand1 + op2;
            set_register(dpi.rd, state, result);
            break;
        case TST:
            result = operand1 & op2;
            break;
        case TEQ:
            result = operand1 ^ op2;
            break;
        case CMP:
            carryBit = op2 <= operand1;
            result = operand1 - op2;
            break;
        case ORR:
            result = operand1 | op2;
            set_register(dpi.rd, state, result);
            break;
        case MOV:
            result = op2;
            set_register(dpi.rd, state, result);
            break;
        default:
            fprintf(stderr, "An unknown operand has been found at PC:%0x.\n", get_register(PC_REG, state));
            print_system_state(state);
            exit(EXIT_FAILURE);
    }
    if (dpi.setBit){
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
            currentCpsr |= (ZERO_FLAG << SHIFT_COND);
        }
        if (is_negative(res)) {
            currentCpsr |= (NEGATIVE_FLAG << SHIFT_COND);
        }
        state->registers[CPSR_REG] = currentCpsr;
    }
}

void execute_sdti(machineState *state, sdtInstruction sdti) {
    uint32_t offset;
    if (sdti.immediate){
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
        } else{
            set_word(state, includingOffset, get_register(sdti.rd, state));
        }
    } else {
        if (sdti.loadBit) {
            set_register(sdti.rd, state, get_word(state, rnContents));
        } else{
            set_word(state, rnContents, get_register(sdti.rd, state));
        }
        set_register(sdti.rn, state, includingOffset);
    }
}


void clear_pipeline(machineState *state) {
    assert(state);
    state->fetchedInstr = false;
    state->decodedInstr = false;
}

void execute_bi(machineState *state, branchInstruction bi) {
    state->registers[PC_REG] += bi.offset;
    clear_pipeline(state);
}

bool check_cond(machineState *state, uint8_t instrCond) {
    // takes the highest 4 bits of the CPSR register (i.e. the cond flags)
    uint32_t cpsrFlags = get_register(CPSR_REG, state) >> SHIFT_COND;
    switch (cpsrFlags) {
        // CSPR FLAGS : VCZN in C
        case AL:
            return true;
        case EQ:
            return instrCond & ZERO_FLAG;
        case NE:
            return !(instrCond & ZERO_FLAG);
        case GE:
            return (instrCond & NEGATIVE_FLAG) == ((instrCond & NEGATIVE_FLAG) >> 3);
        case LT:
            return (instrCond & NEGATIVE_FLAG) != ((instrCond & NEGATIVE_FLAG) >> 3);
        case GT:
            return !(instrCond & ZERO_FLAG) && ((instrCond & NEGATIVE_FLAG) == ((instrCond & NEGATIVE_FLAG) >> 3));
        case LE:
            return !(!(instrCond & ZERO_FLAG) &&
                     ((instrCond & NEGATIVE_FLAG) == ((instrCond & NEGATIVE_FLAG) >> 3)));
            // = !GT
        default:
            fprintf(stderr, "An unsupported instruction has been found at PC: %x\n", get_register(PC_REG, state));
            print_system_state(state);
            exit(EXIT_FAILURE);
    }
}

void execute_instructions(machineState *state) {
    decodedInstruction decoded = state->instructionAfterDecode;
    if (decoded.type == ZERO) {
        printf("A zero instruction has been found at PC: 0x%x and the program will terminate.\n",
               get_register(PC_REG, state));
        print_system_state(state);
        free(state);
        exit(EXIT_SUCCESS);
    }
    if (!check_cond(state, decoded.condCode)) {
        // need to check that will exit function at this point
        return;
    }
    switch (decoded.type) {
        case DATA_PROCESSING:
            execute_dpi(state, decoded.u.dpi);
            break;
        case MULTIPLY:
            execute_mi(state, decoded.u.mi);
            break;
        case SINGLE_DATA_TRANSFER:
            execute_sdti(state, decoded.u.sdti);
            break;
        case BRANCH:
            execute_bi(state, decoded.u.bi);
            break;

        default:
            fprintf(stderr,
                    "An unknown instruction type has been found at PC: 0x%x and the program will terminate.",
                    get_register(PC_REG, state));
            exit(EXIT_FAILURE);
    }
}

void fetch(machineState *state) {
    state->fetched = get_word(state, get_register(PC_REG, state));
    state->fetchedInstr = true;
    state->registers[PC_REG] += 4;
}


/*
bool decoded_instruction_present(machineState *state) {
    if (state->instructionAfterDecode == NULL) {
        return false;
    }
    return state->instructionAfterDecode->type == DATA_PROCESSING
           || state->instructionAfterDecode->type == SINGLE_DATA_TRANSFER
           || state->instructionAfterDecode->type == MULTIPLY
           || state->instructionAfterDecode->type == BRANCH
           || state->instructionAfterDecode->type == ZERO;
}
*/

void advance_program_counter(machineState *state) {
    state->registers[PC_REG] += 4;
}

void pipeline(machineState *state) {
    while (true) {
        if (state->decodedInstr) {
            execute_instructions(state);
        }

        if (state->fetchedInstr) {
            decode(state, state->fetched);
        }

        fetch(state);

    }

}


/*
const machineState default_state = {
        .registers = {0},
        .memory = {0},
        .fetchedInstr = false,
        .fetched = 0,
        .instructionAfterDecode
};
*/


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "You have not started the program with the correct number of inputs.");
        return EXIT_FAILURE;
    }
    machineState *state = (machineState *) calloc(1, sizeof(machineState));
    //*state = default_state;
    read_file(state, argv[1]);
    pipeline(state);


}
