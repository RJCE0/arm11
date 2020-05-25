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
void sdt(struct state_of_machine machine, uint32_t instruction);
void branch(struct state_of_machine machine, uint32_t instruction);
void data_processing(struct state_of_machine machine, uint32_t instruction);
void multiply_instruction(struct state_of_machine machine, uint32_t instruction);
#endif //ARM11_37_EMULATE_H
