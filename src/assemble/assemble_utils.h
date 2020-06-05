#include "assemble_utils.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

#define MULTIPLY_CONDITION_CODE 15
// Created by Tyrell Duku on 03/06/2020.
//

#ifndef ARM11_37_ASSEMBLE_UTILS_H
#define ARM11_37_ASSEMBLE_UTILS_H

int keyfromstring(char *key, instruction *instruction);
uint32_t hex_to_decimal(char hex[]);
uint32_t label_to_instruction(char label[], size_t size);
uint32_t get_label_address(char **labelsArray, char *str);
uint32_t create_branch(uint8_t condCode, uint32_t offset);









#endif //ARM11_37_ASSEMBLE_UTILS_H
