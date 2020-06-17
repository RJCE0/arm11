#include "structs.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

bool is_reg(const char *str) {
    return *str == 'r';
}

bool is_imm(const char *str) {
    return *str == '#';
}

bool check_negative_imm(const char *str) {
    return *(str + 1) == '-';
}

bool check_negative_reg(const char *str) {
    return *str == '-';
}

int get_reg_num(const char *str) {
    return atoi(str + 1);
}

int get_immediate(const char *str) {
    int base = 10;
    if (*(str + 1) == '0' && *(str + 2) == 'x') {
        // for hexadecimal
        base = 0;
    }
    return strtol(str + 1, NULL, base);
}

void check_pre_index(const char *str, uint32_t *preIndexBit) {
    while (*str) {
        if (*str++ == ']') {
            *preIndexBit = 0;
        }
    }
}

uint32_t get_label_address(labelInfo *labels, char *str, int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(labels[i].s, str) == 0) {
            return labels[i].i;
        }
    }
    fprintf(stderr, "Labels does not exist.\n");
    exit(EXIT_FAILURE);
    return -1;
}

void split_on_commas(char *str, instruction *instr) {
    int count = 0;
    char *word = strtok(str, ", ");
    while (word) {
        strcpy(instr->args[count], word);
        count++;
        word = strtok(NULL, ", ");
    }
    instr->argSize = count;
}

void set_instruction(uint32_t *decoded, int pc, uint32_t result) {
    decoded[pc / 4] = result;
}
