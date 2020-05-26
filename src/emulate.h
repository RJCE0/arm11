#include <stdbool.h>
#include "structs.h"
#include "constants.h"

#ifndef ARM11_37_EMULATE_H
#define ARM11_37_EMULATE_H

/*Reads a file with the given filename, loading its contents into an array */
bool read_file (uint8_t dst[], char *filename);
uint32_t get_register(int regNumber, machineState state);
bool set_register(int regNumber, machineState state, uint32_t value);
uint32_t get_memory(uint32_t address, machineState state);
bool set_memory(uint32_t address, machineState state, uint8_t value);
uint32_t get_word(machineState state, uint32_t address);
void print_register_values(machineState state);
void print_system_state(machineState state);
void decode(machineState state, uint32_t instruction);
shiftedRegister opcode_shift_register(machineState state, uint32_t instruction);
void data_processing_instruction( machineState state, uint32_t instruction);
void multiply_instruction( machineState state, uint32_t instruction);
void sdt_instruction( machineState state, uint32_t instruction);
void branch_instruction( machineState state, uint32_t instruction);

/*Checks the first four bits (assuming big endian) of a given instruction align with the CPSR flags of the machine. */
bool check_instruction( machineState state, uint32_t instruction);
void execute_instructions(machineState state);


#endif //ARM11_37_EMULATE_H
