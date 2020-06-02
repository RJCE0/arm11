#include <stdbool.h>
#include "structs.h"
#include "constants.h"
#include <stdint.h>

#ifndef ARM11_37_EMULATE_H
#define ARM11_37_EMULATE_H

/*Reads a file with the given filename, loading its contents into an array */
void read_file(machineState *state, char *filename);
uint32_t get_register(machineState *state, uint32_t regNumber);
void set_register(machineState *state, uint32_t regNumber, uint32_t value);
uint32_t get_word(machineState *state, uint32_t address);
void set_word(machineState *state, uint32_t address, uint32_t word);
void print_register_values(machineState *state);
void print_memory(machineState *state, uint32_t address);
void print_system_state(machineState *state);
void exit_error(machineState *state);
void operand_shift_register(machineState *state, uint16_t instruction, bool *carryBit, uint32_t *operand2);
void decode_dpi(dataProcessingInstruction *dpi, uint32_t instruction);
void decode_mi(multiplyInstruction *mi, uint32_t instruction);
void decode_sdt(sdtInstruction *sdti, uint32_t instruction);
void decode_bi(branchInstruction *bi, uint32_t instruction);
void decode(machineState *state);
void execute_dpi(machineState *state);
void execute_mi(machineState *state);
void execute_sdti(machineState *state);
void execute_bi(machineState *state);
bool check_cond(machineState *state);
void execute_instructions(machineState *state);
void fetch(machineState *state);
void pipeline(machineState *state);



#endif //ARM11_37_EMULATE_H
