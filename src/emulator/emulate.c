#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulate.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

machineState *initialise_state(void){
    machineState *state = (machineState *) calloc(1, sizeof(machineState));
    state->instructionAfterDecode = (decodedInstruction *)
    malloc(sizeof(decodedInstruction));
    state->instructionAfterDecode->type = NULL_INSTR;
    return state;
}

void free_state(machineState *state) {
    print_system_state(state);
    free(state->instructionAfterDecode);
    free(state);
}

void exit_error(machineState *state) {
    free_state(state);
    exit(EXIT_FAILURE);
}

void read_file(machineState *state, char *filename) {
    FILE *binFile;
    binFile = fopen(filename, "rb");
    if (!binFile) {
        fprintf(stderr, "File does not exist. Exiting...\n");
        exit_error(state); /* non-zero val -- couldn't read file */
    }
    /* Elements to be read are each 4 bytes. Binary files can be any size and
    fread will only read till the end of the file or until the size is met.
    Fread also returns the number of elements read.
    */
    fread(state->memory, MEMORY_SIZE, 1, binFile);
    if (ferror(binFile)) {
        fprintf(stderr, "Error while reading file. Exiting...\n");
        exit_error(state);
    }
    fclose(binFile);
}

void check_register(machineState *state, uint32_t regNumber) {
    if (regNumber == 13 || regNumber == 14) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range. Returning NULL.");
        exit_error(state);
    }
}

uint32_t get_register(machineState *state, uint32_t regNumber) {
    check_register(state, regNumber);
    return state->registers[regNumber];
}

void set_register(machineState *state, uint32_t regNumber, uint32_t value) {
    check_register(state, regNumber);
    state->registers[regNumber] = value;
}

static bool check_word(uint32_t address) {
    if (address > MEMORY_SIZE - WORD_SIZE_IN_BYTES) {
        printf("Error: Out of bounds memory access at address 0x%08x\n", address);
        return false;
    }
    return true;
}

uint32_t get_word(machineState *state, uint32_t address) {
    uint32_t fullWord = 0;
    check_word(address);
    // converts from little endian to big endian
    for (size_t i = 0; i < 4; ++i) {
        fullWord += state->memory[address + i] << (i << 3);
    }
    return fullWord;

}

void set_word(machineState *state, uint32_t address, uint32_t value) {
    if (!check_word(address)) {
        exit_error(state);
        return;
    }
    // converts from big endian to little endian
    for (int i = 0; i < 4; ++i) {
        state->memory[address + i] = (value & 0xFF);
        value >>= 8;
    }
}

void print_register_values(machineState *state) {
    for (int i = 0; i < NUM_OF_REGISTERS - 4; ++i) {
            printf("$%-3d: %10d (0x%08x)\n", i,
            state->registers[i], state->registers[i]);
    }
    printf("PC  : %10d (0x%08x)\n", state->registers[PC_REG],
    state->registers[PC_REG]);
    printf("CPSR: %10d (0x%08x)\n", state->registers[CPSR_REG],
    state->registers[CPSR_REG]);
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

void decode_dpi(dataProcessingInstruction *dpi, uint32_t instruction) {
    // if instruction is false then it must contain all zeros so is a zero instruction
    dpi->operand2 = instruction & 0xFFF;
    instruction >>= 12;
    dpi->rd = instruction & 0xF;
    instruction >>= 4;
    dpi->rn = instruction & 0xF;
    instruction >>= 4;
    dpi->setBit = instruction & 0x1;
    instruction >>= 1;
    dpi->opcode = instruction & 0xF;
    instruction >>= 4;
    dpi->immediate = instruction & 0x1;
}

static bool is_negative(uint32_t instruction, int negBit) {
    return (instruction >> negBit) & 0x1;
}

void decode_mi(multiplyInstruction *mi, uint32_t instruction) {
    mi->rm = instruction & 0xF;
    instruction >>= 8;
    mi->rs = instruction & 0xF;
    instruction >>= 4;
    mi->rn = instruction & 0xF;
    instruction >>= 4;
    mi->rd = instruction & 0xF;
    instruction >>= 4;
    mi->setBit = instruction & 0x1;
    instruction >>= 1;
    mi->accumBit = instruction & 0x1;
}

void decode_sdt(sdtInstruction *sdti, uint32_t instruction) {
    sdti->offset = instruction & 0xFFF;
    instruction >>= 12;
    sdti->rd = instruction & 0xF;
    instruction >>= 4;
    sdti->rn = instruction & 0xF;
    instruction >>= 4;
    sdti->loadBit = instruction & 0x1;
    instruction >>= 3;
    sdti->upBit = instruction & 0x1;
    instruction >>= 1;
    sdti->indexingBit = instruction & 0x1;
    instruction >>= 1;
    sdti->immediate = instruction & 0x1;
}

void decode_bi(branchInstruction *bi, uint32_t instruction) {
    uint32_t offset = instruction & 0xFFFFFF;
    // checks if signed number is negative, if so it gets sign extended from 24 to 32 bits
    if (is_negative(offset, 23)) {
        offset |= SE_32;
    }
    offset <<= 2;
    bi->offset = offset;
}

void decode(machineState *state) {
    uint32_t instruction = state->fetched;
    state->instructionAfterDecode->condCode = (instruction >> SHIFT_COND) & 0xF;
    if (!(instruction & 0x600000) && (instruction & 0xC000000) >> 26 == 0x1) {
        state->instructionAfterDecode->type = SINGLE_DATA_TRANSFER;
        decode_sdt(&(state->instructionAfterDecode->u.sdti), instruction);
    } else if ((instruction & 0xF000000) >> 24 == 0xA) {
        state->instructionAfterDecode->type = BRANCH;
        decode_bi(&(state->instructionAfterDecode->u.bi), instruction);
    } else if (!(instruction & 0xFC00000) && ((instruction & 0xF0) >> 4 == 9)) {
        state->instructionAfterDecode->type = MULTIPLY;
        decode_mi(&(state->instructionAfterDecode->u.mi), instruction);
    } else if (!(instruction & 0xC000000)) {
        if (instruction) {
            state->instructionAfterDecode->type = DATA_PROCESSING;
        } else {
            state->instructionAfterDecode->type = ZERO;
        }
        decode_dpi(&(state->instructionAfterDecode->u.dpi), instruction);
    } else {
        printf("Unsupported instruction type to decode at PC: 0x%08x\n",
        state->registers[PC_REG] - 4);
        exit_error(state);
    }
}

// in separate function as it is used if operand2 in dpi is an immediate
void rotate_right(uint32_t contents, uint32_t shiftNum, bool *carryBit,
  uint32_t *operand2){
    *carryBit = (contents >> (shiftNum - 1)) & 0x1;
    *operand2 = (contents >> shiftNum) | (contents << (32 - shiftNum));
}

void operand_shift_register(machineState *state, uint16_t instruction,
  bool *carryBit, uint32_t *operand2) {
    uint32_t rm = instruction & 0xF;
    uint32_t rmContents = get_register(state, rm);
    instruction >>= 4;
    uint32_t shiftType = (instruction & 0x6) >> 1;
    uint32_t shiftNum;
    // checks whether a shift register or integer
    if (instruction & 0x1) {
        instruction >>= 4;
        shiftNum = get_register(state, instruction & 0xF) & 0xF;
    } else {
        instruction >>= 3;
        shiftNum = instruction & 0x1F;
    }
    // if the number to shift by = 0 then rmContents can be return as it is with no carry
    if (!shiftNum){
        *carryBit = 0;
        *operand2 = rmContents;
        return;
    }
    switch (shiftType) {
        case LOGICAL_LEFT:
            *carryBit = (rmContents >> (32 - shiftNum)) & 0x1;
            *operand2 = rmContents << shiftNum;
            break;
        case LOGICAL_RIGHT:
            *carryBit = (rmContents >> (shiftNum - 1)) & 0x1;
            *operand2 = rmContents >> shiftNum;
            break;
        case ARITH_RIGHT: {
            *carryBit = (rmContents >> (shiftNum - 1)) & 0x1;
            uint32_t signBit = rmContents & SIGN_32_BIT;
            // if signBit is = 0 then returns a normal right shift
            if (!signBit) {
                *operand2 = (rmContents >> shiftNum);
                return;
            }
            uint32_t preservedSign = 0;
            // iterates through ensuring the most significant bit is repeated for each right shift along
            for (uint32_t i = 0; i < shiftNum; ++i) {
                preservedSign |= signBit;
                signBit >>= 1;
            }
            *operand2 = (rmContents >> shiftNum) | preservedSign;
            break;
        }
        case ROTATE_RIGHT:
            rotate_right(rmContents, shiftNum, carryBit, operand2);
            break;
        default:
            fprintf(stderr, "An unknown shift has been found.\n");
            exit_error(state);
    }
}

static uint32_t get_cond_codes(machineState *state){
    return (state->registers[CPSR_REG] & 0xF0000000) >> SHIFT_COND;
}


// changes the flags on the CPSR register
void set_flags(machineState *state, flagChange flags[], int size){
    uint32_t currentFlags = get_cond_codes(state);
    for (size_t i = 0; i < size; ++i) {
        if(flags[i].set){
            currentFlags |= flags[i].flag;
        } else {
            currentFlags &= ~flags[i].flag;
        }
    }
    state->registers[CPSR_REG] = currentFlags << SHIFT_COND;
}

void execute_dpi(machineState *state) {
    dataProcessingInstruction *dpi = &(state->instructionAfterDecode->u.dpi);
    uint32_t operand1 = get_register(state, dpi->rn);
    uint32_t operand2;
    bool carryBit;
    uint32_t result;

    if (dpi->immediate) {
        uint32_t imm = dpi->operand2 & 0xFF;
        // >>8 to get the rotate value but * 2 so << 1 therefore >>7 overall
        uint32_t rotate = ((dpi->operand2 & 0xF00) >> 7);
        rotate_right(imm, rotate, &carryBit, &operand2);
    } else {
        operand_shift_register(state, dpi->operand2, &carryBit, &operand2);
    }

    switch (dpi->opcode) {
        case AND:
        case TST:
            result = operand1 & operand2;
            break;
        case EOR:
        case TEQ:
            result = operand1 ^ operand2;
            break;
        case CMP:
        case SUB:
            // if operand2 is less than or equal to operand1 then there won't be a borrow so carryBit = 1
            carryBit = operand2 <= operand1;
            result = operand1 - operand2;
            break;
        case RSB:
            // if operand1 is less than or equal to operand2 then there won't be a borrow so carryBit = 1
            carryBit = operand1 <= operand2;
            result = operand2 - operand1;
            break;
        case ADD:
            // if operand2 is bigger than the difference between uint32 max and operand 1 then there will be an overflow so carryBit = 1
            carryBit = (UINT32_MAX - operand1) < operand2;
            result = operand1 + operand2;
            break;
        case ORR:
            result = operand1 | operand2;
            break;
        case MOV:
            result = operand2;
            break;
        default:
            // will exit as an error if it falls through switch as it should not reach this stage
            fprintf(stderr, "An unknown operand has been found at PC: %0x.\n",
            state->registers[PC_REG] - 8);
            exit_error(state);
    }
    // to reduce code duplication in switch if opcode is not tst, teq, cmp then it will be written to register rd
    if (dpi->opcode < 8 || dpi->opcode > 10){
        set_register(state, dpi->rd, result);
    }
    // if setBit is 1 then carryFlags in CPSR are changed
    if (dpi->setBit) {
        flagChange flags[3];
        flags[0].flag = Z_FLAG;
        flags[0].set = 0;
        if (result == 0) {
            flags[0].set = 1;
        }
        flags[1].flag = N_FLAG;
        flags[1].set = result & SIGN_32_BIT;
        flags[2].flag = C_FLAG;
        flags[2].set = carryBit;
        set_flags(state, flags, 3);
    }
}

void execute_mi(machineState *state) {
    multiplyInstruction *mi = &(state->instructionAfterDecode->u.mi);
    uint32_t res = 0;
    uint32_t acc = 0;

    /*Performing the operation */
    acc = (mi->accumBit) ? get_register(state, mi->rn) : 0;
    res = (get_register(state, mi->rm) * get_register(state, mi->rs)) + acc;
    set_register(state, mi->rd, res);

    /*Changing flag status if necessary */
    if (mi->setBit) {
        flagChange flags[2];
        flags[0].flag = Z_FLAG;
        flags[0].set = (res == 0);
        flags[1].flag = N_FLAG;
        flags[1].set = is_negative(res, 31);
        set_flags(state, flags, 2);
    }
}

static void load(machineState *state, uint32_t destReg, uint32_t address){
    // if address is out of range will print error and not load a value into the dest register
    if (!check_word(address)) {
        return;
    }
    set_register(state, destReg, get_word(state, address));
}

void execute_sdti(machineState *state) {
    sdtInstruction *sdti = &(state->instructionAfterDecode->u.sdti);
    uint32_t offset;
    bool carryBit;
    // shifts offset if immediate bit is 1 if not then extends to unsigned 32bit int
    if (sdti->immediate) {
        operand_shift_register(state, sdti->offset, &carryBit, &offset);
    } else {
        offset = sdti->offset;
    }
    uint32_t rnContents = get_register(state, sdti->rn);
    uint32_t includingOffset;
    // +/- offset to the contents of register rn depending on upBit
    if (sdti->upBit) {
        includingOffset = rnContents + offset;
    } else {
        includingOffset = rnContents - offset;
    }
    // if true then preIndexing occurs so memory address accessed includes offset
    if (sdti->indexingBit) {
        if (sdti->loadBit) {
            load(state, sdti->rd, includingOffset);
        } else {
            set_word(state, includingOffset, get_register(state, sdti->rd));
        }
    } else {
        if (sdti->loadBit) {
            load(state, sdti->rd, rnContents);
        } else {
            set_word(state, rnContents, get_register(state, sdti->rd));
        }
        // post indexing so includingoffset is written to base register
        // don't need to call set_register function as register number has been previously checked for rnContents
        state->registers[sdti->rn] = includingOffset;
    }
}

static void clear_pipeline(machineState *state) {
    // resets pipeline to how it is in the first fetch, decode, execute cycle
    assert(state);
    state->fetchedInstr = false;
    state->instructionAfterDecode->type = NULL_INSTR;
}

void execute_bi(machineState *state) {
    branchInstruction *bi = &(state->instructionAfterDecode->u.bi);
    if (is_negative(bi->offset, 31)) {
        // if negative then it converts from 2's complement
        state->registers[PC_REG] -= ~(bi->offset - 1);
    } else {
        state->registers[PC_REG] += bi->offset;
    }
    clear_pipeline(state);
}

bool check_cond(machineState *state) {
    uint8_t instrCond = state->instructionAfterDecode->condCode;
    // takes the highest 4 bits of the CPSR register (i.e. the cond flags)
    uint32_t cpsrFlags = get_cond_codes(state);
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
            return !(cpsrFlags & Z_FLAG) && ((cpsrFlags & N_FLAG)
            == (cpsrFlags & V_FLAG));
        case LE:
            return (cpsrFlags & Z_FLAG) || ((cpsrFlags & N_FLAG)
            != (cpsrFlags & V_FLAG));
        case AL:
            return true;
        default:
            // should not reach here unless earlier bug with assigning flags
            fprintf(stderr, "An unsupported instruction (unknown cond. code) has been found at PC: %d (%08x)\n",
            state->registers[PC_REG], state->registers[PC_REG]);
            exit_error(state);
            return false;
    }
}

void execute_instructions(machineState *state) {
    // exit when ZERO instruction is reached
    if (state->instructionAfterDecode->type == ZERO) {
        free_state(state);
        exit(EXIT_SUCCESS);
    }
    // checks condition flags of instruction with CPSR reg, if function returns false then instruction is ignored and not executed
    if (!check_cond(state)) {
        return;
    }
    switch (state->instructionAfterDecode->type) {
        case DATA_PROCESSING:
            execute_dpi(state);
            break;
        case MULTIPLY:
            execute_mi(state);
            break;
        case SINGLE_DATA_TRANSFER:
            execute_sdti(state);
            break;
        case BRANCH:
            execute_bi(state);
            break;
        default:
            // instructions should not reach this stage unless error in fetch/decode
            fprintf(stderr,
            "An unknown instruction type has been found at PC: 0x%x and the program will terminate.",
            state->registers[PC_REG]);
            exit(EXIT_FAILURE);
    }
}

void fetch(machineState *state) {
    // fetches instruction from memory based on PC and makes bool true so next cycle it will decode
    state->fetched = get_word(state, state->registers[PC_REG]);
    state->fetchedInstr = true;
}

void pipeline(machineState *state) {
    // will repeat fetch decode execute cycle until ZERO instruction or invalid instruction
    while (true) {
        // first checks whether there has been a decoded instruction in previous cycle before executing
        if (state->instructionAfterDecode->type != NULL_INSTR) {
            execute_instructions(state);
        }
        // first checks whether there has been a fetched instruction in previous cycle before decoding
        if (state->fetchedInstr){
            decode(state);
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
    // initialises memory for machinestate and decodedinstruction
    machineState *state = initialise_state();
    // reads bin file and stores it into memory in our machine state
    read_file(state, argv[1]);
    pipeline(state);
}
