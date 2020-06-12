#include "assemble_utils.h"

// [Function_A, Function_B, ...]
// [0x00012, 0x00000128]

/* (RJ) branch bit explained for myself:
so for branching, when i see a branch condition it will be made up of two components
the type of branch and a target address. The target address might be an actual address
in or it might be a label, so I'll just use my function I built.
*/




void shift(uint32_t *regNum, char *shiftOp, char *offset) {
    uint32_t shiftType = shift_key(shiftOp) << 5;
    uint32_t shiftNum = 0;
    uint32_t regBit = 0;
    if (is_reg(offset)) {
        shiftNum = (get_reg_num(offset) & 0xF) << 8;
        regBit = 0x10;
    } else {
        shiftNum = (get_immediate(offset) & 0x1F) << 7;
    }
    *regNum |= shiftNum | shiftType | regBit;
}

void operand2_checker(char **args, uint32_t *operand2, uint32_t *immediate, int size) {
    if (is_reg(args[0])) {
        *operand2 = get_reg_num(args[0]) & 0xF;
        if (size >= 2) {
            shift(operand2, args[1], args[2]);
        }
    } else {
        *immediate = 1 << 25;
        uint32_t imm = get_immediate(args[0]);
        uint32_t rotate = 0;
        while (imm & 0xFFFFFF00) {
          imm = imm << 2 | imm >> 30;
          rotate += 1;
        }
        *operand2 = rotate << 8 | imm;
    }
}

void data_processing(instruction *instr, state *curr) {
    uint32_t condCode = 14 << SHIFT_COND; //shift_cond
    uint32_t immediate = 0;
    uint32_t opcode = instr->u.opcode << 21;
    uint32_t setBit = 0;
    uint32_t rn = 0;
    uint32_t rd = 0;
    uint32_t operand2;
    if (instr->u.opcode == 13) {
        rd = get_reg_num(instr->args[0]) << 12;
        operand2_checker(instr->args + 1, &operand2, &immediate, instr->argSize - 1);
    } else if ((instr->u.opcode <= 10) && (instr->u.opcode >= 8)) {
        rn = get_reg_num(instr->args[0]) << 16;
        setBit = 1 << 20;
        operand2_checker(instr->args + 1, &operand2, &immediate, instr->argSize - 1);
    } else {
        rd = get_reg_num(instr->args[0]) << 12;
        rn = get_reg_num(instr->args[1]) << 16;
        operand2_checker(instr->args + 2, &operand2, &immediate, instr->argSize - 2);
    }
    uint32_t result = condCode | immediate | opcode | setBit | rn | rd | operand2;
    curr->decoded[curr->pc / 4] = result;
}


void multiply(instruction *instr, state *curr) {
    uint32_t fullInstr;
    uint32_t condition_code = MULTIPLY_CONDITION_CODE << SHIFT_COND;
    uint32_t accBit = instr->u.accBit << 21;
    uint32_t rd = get_reg_num(instr->args[0]) << 16;
    uint32_t rn = (instr->u.accBit) ? get_reg_num(instr->args[3]) << 12 : 0;
    uint32_t rm = get_reg_num(instr->args[1]);
    // do you need shift?
    uint32_t constant = MULITPLY_BITS_4_THROUGH_7 << 4;
    uint32_t rs = get_reg_num(instr->args[2]) << 8;
    fullInstr = condition_code | accBit | rd | rn | rs | constant | rm;
    curr->decoded[curr->pc / 4] = fullInstr;
}

void single_data_transfer(instruction *instr, state *curr) {
    const uint32_t condCode = 14 << SHIFT_COND;
    uint32_t immBit = 0;
    uint32_t preIndexBit = 1;
    uint32_t upBit = 1;
    uint32_t rn = 0;
    uint32_t rd = 0;
    uint32_t offset = 0;
    rd = get_reg_num(instr->args[0]);
    // <=expression> type (ldr)
    /* Assuming I don't need to take into account if any other instruction has
    already been stored here */
    if (*instr->args[1] == '=') {

        uint32_t expression = get_immediate(instr->args[1]);
        if (expression <= 0xFF) {
            instr->u.opcode = MOV;
            // unnecessary but to avoid bugs
            *instr->args[1] = '#';
            data_processing(instr, curr);
            return;
        } else {
            offset = curr->lastAddress - curr->pc - 8;
            rn = 0xF;
            curr->decoded[curr->lastAddress/4] = expression;
            curr->lastAddress += 4;
						curr->decoded = (uint32_t *) realloc(curr->decoded, (curr->lastAddress/4 + 1) * sizeof(uint32_t));
        }
    }
    // [Rn] case
    // only other case with only two arguments
    // TODO: assuming any set args can never be zero?
    // else if (*(instr->args[1] == '[' && args[2] == 0)) {
    else if (instr->argSize == 2) {
        // +1 to ignore first square bracket in string
        rn = get_reg_num(instr->args[1] + 1);
    } else {
        /* this will compare to check whether this is the pre-indexed case
         or the post indexed case (both with some <#expression>) */
        char *tempPtr = instr->args[1];
        while (*tempPtr) {
            if (*tempPtr++ == ']') {
                preIndexBit = 0;
            }
        }
        rn = get_reg_num(instr->args[1] + 1);
        if (is_reg(instr->args[2])) {
          offset = get_reg_num(instr->args[2]);
          immBit = 1;
        } else {
          if (check_negative_imm(instr->args[2])) {
            upBit = 0;
          }
          offset = get_immediate(instr->args[2]);
        }
        if (instr->argSize > 3) {
          shift(&offset, instr->args[3], instr->args[4]);
        }
    }

    uint32_t result = condCode | (1 << 26) | (immBit << 25) | (preIndexBit << 24)
            | (upBit << 23) | (instr->u.loadBit << 20) | (rn << 16) | (rd << 12) | offset;
    curr->decoded[curr->pc / 4] = result;
}

void branch(instruction *instr, state *curr) {
    int32_t offset;
    uint32_t address;
    if (get_label_address(curr, instr->args[0], &address)) {
        offset = address - curr->pc - 8;
    } else {
        offset = hex_to_decimal(instr->args[0]) - curr->pc - 8;
    }
    offset >>= 2;
    uint32_t result = instr->u.condCode << 28 | 10 << 24 | (offset & 0xFFFFFF);
    curr->decoded[curr->pc / 4] = result;
}

void halt(instruction *instr, state *curr) {
    curr->decoded[curr->pc / 4] = 0;
}



void read_file_first(firstFile *firstRead, char *inputFileName) {
    FILE *file;
    file = fopen(inputFileName, "r");
    if (file == NULL) {
        printf("file not found, exiting...\n");
        return;
    }
    int line = 0;
    int labelCount = 0;
    char str[511];
    while (fgets(str, 511, file)) {
        if (str[strlen(str) - 2] == ':') {
            str[strlen(str) - 2] = '\0';
						firstRead->labels = (labelInfo *) realloc(firstRead->labels, (labelCount + 1) * sizeof(labelInfo));
						firstRead->labels[labelCount].s = (char *) malloc(10 * sizeof(char));
            strcpy(firstRead->labels[labelCount].s, str);
            firstRead->labels[labelCount].i = (line - labelCount) * 4;
            labelCount++;
            // to implement realloc
        }
        if (str[0] != '\n') {
          line++;
        }

    }
    fclose(file);
    firstRead->lines = line - labelCount;
		firstRead->labelCount = labelCount;
}

void split_on_commas(char *input, instruction *instr) {
    int count = 0;
    char *pch = strtok(input, ", ");
		strcpy(instr->args[count], pch);
		for (int i = instr->argSize - 1; i > 0; i--) {
			free(instr->args[i]);
		}
    while (pch != NULL) {
        pch = strtok(NULL, ", ");
        count++;
				if (!pch) {
					instr->argSize = count;
					printf("%d exiting\n", count);
					break;
				}
				instr->args = (char **) realloc(instr->args, (count + 1) * sizeof(char *));
				instr->args[count] = (char *) malloc(20 * sizeof(char));
        strcpy(instr->args[count], pch);
    }
}

state *initalise_state(firstFile *firstRead){
    state *curr = (state *) malloc(sizeof(state));
    curr->labels = firstRead->labels;
    curr->lastAddress = firstRead->lines * 4;
		curr->pc = 0;
		curr->decoded = (uint32_t *) malloc((curr->lastAddress/4 + 1) * sizeof(uint32_t));
    return curr;
}

void free_state(state *curr, int size){
    for (int i = 0; i < size; i++) {
        free(curr->labels[i].s);
    }
    free(curr->labels);
    free(curr->decoded);
    free(curr);
}

instruction *initalise_instruction(void){
    instruction *instr = (instruction *) malloc(sizeof(instruction));
    instr->args = (char **) malloc(sizeof(char *));
    instr->args[0] = (char *) calloc(20, sizeof(char));
		instr->argSize = 1;
    return instr;
}


void free_instruction(instruction *instr){
    for (int i = 0; i < instr->argSize; i++) {
        free(instr->args[i]);
    }
    free(instr->args);
    free(instr);
}

void(*func[NUM_INSTRUCTION])(instruction *instr, state *curr) = {
    data_processing,
    multiply,
    single_data_transfer,
    branch,
    halt
};

void read_file_second(state *curr, char *inputFileName) {
    FILE *file;
    file = fopen(inputFileName, "r");
    if (file == NULL) {
        printf("file not found, exiting...\n");
        return;
    }
    instruction *instr = initalise_instruction();
    // need counter in first read
    char str[511];
    while (fgets(str, 511, file)) {
        char *argsInInstruction;
        char *ptrToFirstSpace = strchr(str, ' ');
        if (ptrToFirstSpace) {
            argsInInstruction = ptrToFirstSpace + 1;
            *ptrToFirstSpace = '\0';
            printf("check\n");
            split_on_commas(strtok(argsInInstruction, "\n"), instr);
        }
        int abstractData = keyfromstring(str, instr);
        if (abstractData != -1) {
            func[abstractData](instr, curr);
            if (!curr->decoded[curr->pc / 4]) {
                break;
            }
            curr->pc += 4;
        }
    }
		fclose(file);
    free_instruction(instr);
}

/*
Need a function that will split the arguments, on commas. We will be taking in
the full line on the fscanf but won't be able to differentiate between arguments.
it would be best to store these in a struct so we can access each part of the
arguments separately. Those who won't need 3 arguments, initalise the others to null.
*/

//add r1, r2, #0x39

firstFile *initalise_first_file(void){
    firstFile *firstRead = (firstFile *) malloc(sizeof(firstFile));
    firstRead->labels = (labelInfo *) malloc(sizeof(labelInfo));
    return firstRead;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Program does not have correct number of arguments.");
        exit(EXIT_FAILURE);
    }
    firstFile *firstRead = initalise_first_file();
    read_file_first(firstRead, argv[1]);
    state *curr = initalise_state(firstRead);
    read_file_second(curr, argv[1]);
    FILE *binFile;
    binFile = fopen(argv[2], "wb");
    fwrite(curr->decoded, sizeof(uint32_t), curr->lastAddress/4, binFile);
    fclose(binFile);
    free_state(curr, firstRead->labelCount);
		free(firstRead);
    return 0;
}
