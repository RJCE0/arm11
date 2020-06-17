#include "utils/helpers.c"
#include "utils/memory_alloc.c"
#include "utils/dictionaries.c"

#ifndef ASSEMBLE
#define ASSEMBLE

void shift(uint32_t *regNum, char *shiftOp, char *offset);
void operand2_checker(char **args, uint32_t *operand2, uint32_t *immediate,
	int size);
void data_processing(instruction *instr, state *curr);
void multiply(instruction *instr, state *curr);
void single_data_transfer(instruction *instr, state *curr);
void branch(instruction *instr, state *curr);
void halt(instruction *instr, state *curr);
void read_file_first(firstFile *firstRead, char *inputFileName);
void(*abstract_type[NUM_INSTRUCTION])(instruction *instr, state *curr) = {
    data_processing,
    multiply,
    single_data_transfer,
    branch,
    halt
};
void read_file_second(state *curr, char *inputFileName);
void write_file(state *curr, char *fileName);


#endif
