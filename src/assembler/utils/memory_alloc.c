#include "structs.h"
#include <stdlib.h>

state *initalise_state(firstFile *firstRead) {
    state *curr = (state *) malloc(sizeof(state));
    curr->labels = firstRead->labels;
    curr->lastInstr = firstRead->lines;
    curr->labelCount = firstRead->labelCount;
    curr->pc = 0;
    curr->decoded = (uint32_t *) malloc((curr->lastInstr + 1) * sizeof(uint32_t));
    return curr;
}

void free_state(state *curr, int size) {
    for (int i = 0; i < size; i++) {
        free(curr->labels[i].s);
    }
    free(curr->labels);
    free(curr->decoded);
    free(curr);
}

instruction *initalise_instruction(void) {
    instruction *instr = (instruction *) malloc(sizeof(instruction));
    instr->args = (char **) malloc(MAX_ARG_SIZE * sizeof(char *));
    for (int i = 0; i < MAX_ARG_SIZE; i++) {
        instr->args[i] = (char *) calloc(20, sizeof(char));
    }
    instr->argSize = 1;
    return instr;
}

void free_instruction(instruction *instr) {
    for (int i = 0; i < MAX_ARG_SIZE; i++) {
        free(instr->args[i]);
    }
    free(instr->args);
    free(instr);
}

firstFile *initalise_first_file(void) {
    firstFile *firstRead = (firstFile *) malloc(sizeof(firstFile));
    firstRead->labels = (labelInfo *) malloc(sizeof(labelInfo));
    return firstRead;
}

labelInfo *realloc_labels(labelInfo *labels, int labelCount) {
    labels = (labelInfo *) realloc(labels, (labelCount + 1) * sizeof(labelInfo));
    labels[labelCount].s = (char *) malloc(10 * sizeof(char));
    return labels;
}

uint32_t *store_exp(uint32_t *decoded, int *last, uint32_t expression) {
    decoded[*last] = expression;
    *last += 1;
    decoded = (uint32_t *) realloc(decoded, (*last + 1) * sizeof(uint32_t));
    return decoded;
}
