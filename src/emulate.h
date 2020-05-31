#include <stdbool.h>
#include "structs.h"
#include "constants.h"
#include <stdint.h>

#ifndef ARM11_37_EMULATE_H
#define ARM11_37_EMULATE_H

/*Reads a file with the given filename, loading its contents into an array */
bool read_file(machineState *state, char *filename);
uint32_t get_register(uint32_t regNumber, machineState *state);
bool set_register(uint32_t regNumber, machineState *state, uint32_t value);
uint32_t get_word(machineState *state, uint32_t address);
bool set_word(machineState *state, uint32_t address, uint32_t word);
void print_register_values(machineState *state);
void print_memory(machineState *state, uint32_t address);
void print_system_state(machineState *state);
void exit_error(machineState *state);
shifted operand_shift_register(machineState *state, uint16_t instruction);
dataProcessingInstruction decode_dpi(machineState *state, uint32_t instruction);
multiplyInstruction decode_mi(machineState *state, uint32_t instruction);
sdtInstruction decode_sdt(machineState *state, uint32_t instruction);
branchInstruction decode_bi(machineState *state, uint32_t instruction);
void decode(machineState *state, uint32_t instruction);
void execute_dpi(machineState *state);
void execute_mi(machineState *state);
void execute_sdti(machineState *state);
void execute_bi(machineState *state);
bool check_cond(machineState *state);
void execute_instructions(machineState *state);
void fetch(machineState *state);
void pipeline(machineState *state);



#endif //ARM11_37_EMULATE_H
