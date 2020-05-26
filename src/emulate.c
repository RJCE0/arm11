#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulate.h"
#include <assert.h>
#include <string.h>

/*The plan for emulator:
 * 1: Load in the binary file, gives 32-bit words. Check the first four bits and compare with
 * first four bits of CPSR register.
 *
 * 2: If they match, prepare to start decoding, if not move to next instruction.
 *
 * 3: Fetch -> Decode -> Execute cycle until we get all zero instruction.
 */


 bool read_file(machineState state, char *filename) {
     FILE *binFile;
     binFile = fopen(filename, "rb");
     if (binFile == NULL) {
         fprintf(stderr, "File does not exist. Exiting...\n");
         return EXIT_FAILURE; /* non-zero val -- couldn't read file */
     }
     /* Elements to be read are each 4 bytes. Binary files can be any size and
     fread will only read till the end of the file or until the size is met.
     Fread also returns the number of elements read.
     */
     fread(state.memory, MEMORY_SIZE, 1, binFile);
     fclose(binFile);
     return true;
}

uint32_t get_register(int regNumber, machineState state) {
    if (regNumber == 13 || regNumber == 14 || regNumber < 0 || regNumber > 16) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range. Returning NULL.");
        return NULL;
    }
    return state.registers[regNumber];
}

bool set_register(int regNumber, machineState state, uint32_t value) {
    if (regNumber == 13 || regNumber == 14 || regNumber < 1 || regNumber > 16) {
        fprintf(stderr, "Invalid register number specified, not supported or out of range.");
        return false;
    }
    state.registers[regNumber] = value;
    return true;
}

uint32_t get_memory(uint32_t address, machineState state) {
    return state.memory[address];
}

bool set_memory(uint32_t address, machineState state, uint8_t value) {
    // future improvement: allow writing of only part of a value (using some sort of truncation)
    if (address > 65532) {
        fprintf(stderr,"Address specified is too high. Segmentation fault detected.");
        return false;
    }
    state.memory[address] = value;
    return true;
}

uint32_t get_word(machineState state, uint32_t address) {
    uint32_t fullWord = 0;
    memcpy(&fullWord,&state.memory[address], WORD_SIZE_IN_BYTES);
    return fullWord;
    // Might be an issue here...
}

static void print_binary_array(uint8_t array[], size_t size) {
    for (int i = 0; i < size; i++) {
        printf("%c", array[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
    }
}

void print_register_values(machineState state) {
    for (int i = 0; i < NUM_OF_REGISTERS; i++) {
        if (i != 13 || i != 14) {
        printf("Register %i : 0x%x", (i+1), getRegister(i, state));
        }
    }
}

void print_system_state(machineState state) {
    print_binary_array(state.memory, WORD_SIZE_IN_BYTES);
    print_register_values(state);
}

void decode(machineState state, uint32_t instruction) {
    uint32_t mask = 0xFFFFFFF;
    instruction &= mask;
    // Removed condition bits from instruction.
    if (instruction >> 26 == 1) {
        sdt_instruction(state, instruction);
    } else if (instruction >> 24 == 10) {
        branch_instruction(state, instruction);
    } else if (((instruction >> 22) | (instruction >> 4)) == 9) {
        multiply_instruction(state, instruction);
    } else if ((instruction >> 26) == 0) {
        data_processing_instruction(state, instruction);
    } else {
         fprintf(stderr, "An unsupported instruction has been found at [PC: %x] and the program can no longer continue. Exiting...\n", state.registers[PC_REG]);
         print_system_state(state);
         free(state.registers);
         free(state.memory);
         exit(EXIT_FAILURE);
    }
}

shiftedRegister opcode_shift_register(machineState state, uint32_t instruction){
    uint32_t rm = instruction & 0xF;
    uint32_t rm_contents = get_register(rm, state);
    uint32_t shift_num = (instruction >> 7) & 0x1F;
    uint32_t shift_type = (instruction >> 5) & 0x3;
    shiftedRegister result;
    switch (shift_type) {
      case logicalLeft:
          result.opcode2 = rm_contents << shift_num;
          result.carryBit = (rm_contents >> (32 - shift_num)) & 0x1;
          return result;
          break;
      case logicalRight:
          result.opcode2 = rm_contents >> shift_num;
          result.carryBit = (rm_contents >> (shift_num - 1)) & 0x1;
          return result;
          break;
      case arithRight:
          uint32_t preservedSign;
          uint32_t signBit = rm_contents & 0x80000000;
          for (int i = 0; i < shift_num; i++) {
              preservedSign += signBit;
              signBit >>= 1;
          }
          result.opcode2 = (rm_contents >> shift_num) + preservedSign;
          result.carryBit = (rm_contents >> (shift_num - 1)) & 0x1;
          return result;
          break;
      case rotateRight:
          result.opcode2 = (rm_contents >> shift_num) | (rm_contents << (32 - shift_num));
          result.carryBit = (rm_contents >> (shift_num - 1)) & 0x1;
          return result;
          break;
      default:
          return result;
  }
}

void data_processing_instruction( machineState state, uint32_t instruction){

}

static bool is_negative(uint32_t instruction) {
    return (instruction & 1) != 0;
}

void multiply_instruction(machineState state, uint32_t instruction) {
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
    acc = (aFlag) ? state.registers[rn] : 0;
    res = (state.registers[rm] * state.registers[rs]) + acc;
    state.registers[rd] = res;

    /*Changing flag status if necessary */
    if (sFlag) {
         currentCpsr = state.registers[CPSR_REG] & 0xFFFFFFF;
         lastBit = res &= (0x1 >> 31);
        if (res == 0) {
            /* VCZN */
            currentCpsr |= (zeroFlag << SHIFT_COND);
        }
        if (is_negative(res)) {
        currentCpsr |= (negativeFlag << SHIFT_COND);
        }
        state.registers[CPSR_REG] = currentCpsr;
    }
}


void sdt_instruction(machineState state, uint32_t instruction) {
  bool offsetBit = (((instruction >> 25) & 1) == 1);
  bool preindexingBit = (((instruction >> 24) & 1) == 1);
  bool upBit = (((instruction >> 23) & 1) == 1);
  bool loadBit = (((instruction >> 20) & 1) == 1);
  bool baseRegNum = (((instruction >> 16) & 15) == 1);
  bool srcDestRegNum = (((instruction >> 12) & 15) == 1);
  uint32_t offset;

  // 4095 represents a mask of the least significant 12 bits
  if (!offsetBit) {
      offset = ((instruction)&4095);
  } else {
      // Implement
      // offset obtained using functionality from data processing instr.
      offset = 0;
  }

  offset = instruction & 4095;
  uint32_t baseRegVal = get_register(baseRegNum, state);
  uint32_t srcDestRegVal = get_register(srcDestRegNum, state);

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

void branch_instruction(machineState state, uint32_t instruction) {
    uint32_t offset = instruction & 0xFFFFFF;
    offset <<= 2;

    if (isNegative(instruction >> 23)) {
        offset |= SIGNEXTENSION__TO_32;
    }
    state.registers[PC_REG] += offset;
}

bool check_instruction(machineState state, uint32_t instruction) {
    instruction >>= SHIFT_COND;
    char cpsrFlags = state.registers[CPSR_REG] >> SHIFT_COND;
    switch (instruction) {
        // CSPR FLAGS : VCZN in C
        case AL:
            state.has_instruction = true;
            return true;
        case EQ:
            state.has_instruction = true;
            return cpsrFlags & zeroFlag;
        case NE:
            state.has_instruction = true;
            return !(cpsrFlags & zeroFlag);
        case GE:
            state.has_instruction = true;
            return (cpsrFlags & negativeFlag) == ((cpsrFlags & negativeFlag) >> 3);
        case LT:
            state.has_instruction = true;
            return (cpsrFlags & negativeFlag) != ((cpsrFlags & negativeFlag) >> 3);
        case GT:
            state.has_instruction = true;
            return !(cpsrFlags & zeroFlag) && ((cpsrFlags & negativeFlag) == ((cpsrFlags & negativeFlag) >> 3));
        case LE:
            state.has_instruction = true;
            return !(!(cpsrFlags & zeroFlag) && ((cpsrFlags & negativeFlag) == ((cpsrFlags & negativeFlag) >> 3)));
        default:
            fprintf(stderr, "An unsupported instruction has been found at PC: %x", state.registers[PC_REG]);
            print_system_state(state);
            exit(EXIT_FAILURE);
    }
}

void execute_instructions(machineState state) {
    uint32_t programCounter = getWord(state, getRegister(PC_REG, state));
    while (programCounter != 0) {
        decode(state, programCounter);
        programCounter += 4;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "You have not started the program with the correct number of inputs.");
        return EXIT_FAILURE;
    }

     machineState state = {(uint8_t *) calloc (MEMORY_SIZE, 1),
      (uint32_t *) calloc (NUM_OF_REGISTERS,sizeof(uint32_t)), false};

    // size of 1 allows memory to be byte addressable
    read_file(state, argv[1]);
    execute_instructions(state);
    // printBinaryArray(memory, 100);
    return 0;
}
