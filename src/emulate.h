//
// Created by Tyrell Duku on 23/05/2020.
//

#include <stdbool.h>
#include "state_of_machine.h"
#include "constants.h"

#ifndef ARM11_37_EMULATE_H
#define ARM11_37_EMULATE_H
/*Checks the first four bits (assuming big endian) of a given instruction align with the CPSR flags of the machine. */
bool check_instruction(struct state_of_machine machine, uint32_t instruction);
/*Reads a file with the given filename, loading its contents into an array */
bool read_fiie (uint8_t dst[], char *filename);

void decode(struct state_of_machine machine, uint32_t instruction);
void sdtInstruction(struct state_of_machine machine, uint32_t instruction);
void branchInstruction(struct state_of_machine machine, uint32_t instruction);
void dataProcessingInstruction(struct state_of_machine machine, uint32_t instruction);
void multiplyInstruction(struct state_of_machine machine, uint32_t instruction);
uint32_t getRegister(int regNumber, struct state_of_machine state);
bool setRegister(int regNumber, struct state_of_machine state, uint32_t value);
uint32_t getMemory(uint32_t address, struct state_of_machine state);
bool setMemory(uint32_t address, struct state_of_machine state, uint8_t value);
uint32_t getWord(struct state_of_machine machine, uint32_t address);
void printSystemState(struct state_of_machine state);

#endif //ARM11_37_EMULATE_H
