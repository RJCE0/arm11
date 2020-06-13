#include "assemble_utils.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

#define MULTIPLY_CONDITION_CODE 14
#define MULITPLY_BITS_4_THROUGH_7 9
// Created by Tyrell Duku on 03/06/2020.
//

#ifndef ARM11_37_ASSEMBLE_UTILS_H
#define ARM11_37_ASSEMBLE_UTILS_H

int keyfromstring(char *key, instruction *instruction);

int32_t hex_to_decimal(const char *hex);

bool get_label_address(state *curr, char *str, uint32_t *address);

uint32_t create_branch(uint8_t condCode, int32_t offset);


#endif //ARM11_37_ASSEMBLE_UTILS_H
