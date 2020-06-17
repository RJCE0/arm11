#include "assemble.h"

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
        *immediate = IMM_BIT;
        uint32_t imm = get_immediate(args[0]);
        uint32_t rotate = 0;
		// opposite of rotate right so its a rotate left
        while (imm & 0xFFFFFF00) {
            imm = imm << 2 | imm >> 30;
            rotate += 1;
        }
        *operand2 = rotate << 8 | imm;
    }
}

void data_processing(instruction *instr, state *curr) {
    uint32_t immediate = 0;
    uint32_t opcode = instr->u.opcode << 21;
    uint32_t setBit = 0;
    uint32_t rn = 0;
    uint32_t rd = 0;
    uint32_t operand2;
    int argOffset;
    // to make code more readable
    char **args = instr->args;
    // for mov case
    if (instr->u.opcode == 13) {
        rd = get_reg_num(args[0]) << 12;
        argOffset = 1;
    // for tst, teq and cmp
    } else if ((instr->u.opcode <= 10) && (instr->u.opcode >= 8)) {
        rn = get_reg_num(args[0]) << 16;
        setBit = DPI_S_BIT;
        argOffset = 1;
    // all other cases
    } else {
        rd = get_reg_num(args[0]) << 12;
        rn = get_reg_num(args[1]) << 16;
        argOffset = 2;
    }
    operand2_checker(args + argOffset, &operand2, &immediate,
        instr->argSize - argOffset);
    uint32_t result = AL_COND | immediate | opcode | setBit | rn | rd |
    operand2;
	set_instruction(curr->decoded, curr->pc, result);
}


void multiply(instruction *instr, state *curr) {
    // to make code more readable
    char **args = instr->args;
    uint32_t accBit = instr->u.accBit << 21;
    uint32_t rd = get_reg_num(args[0]) << 16;
    uint32_t rn = (instr->u.accBit) ? get_reg_num(args[3]) << 12 : 0;
    uint32_t rm = get_reg_num(args[1]);
    uint32_t rs = get_reg_num(args[2]) << 8;
    uint32_t result = AL_COND | accBit | rd | rn | rs | MULTIPLY_CONST | rm;
	set_instruction(curr->decoded, curr->pc, result);
}

void single_data_transfer(instruction *instr, state *curr) {
    uint32_t immBit = 0;
    uint32_t loadBit = instr->u.loadBit << 20;
    uint32_t preIndexBit = SDT_P_BIT;
    uint32_t upBit = SDT_U_BIT;
    uint32_t rn = 0;
    uint32_t offset = 0;
    int negOffset = 0;
    // to make code more readable
    char **args = instr->args;
    uint32_t rd = get_reg_num(args[0]) << 12;
    // <=expression> type (ldr)
    if (*instr->args[1] == '=') {
        uint32_t expression = get_immediate(args[1]);
        // if less than 0xFF then converted to MOV
        if (expression <= 0xFF) {
            instr->u.opcode = MOV;
            // unnecessary but to avoid bugs
            *args[1] = '#';
            data_processing(instr, curr);
            return;
        } else {
            offset = (curr->lastInstr * 4) - curr->pc - 8;
            rn = 0xF << 16;
            // appended to end of decoded instructions array
            curr->decoded = store_exp(curr->decoded, &curr->lastInstr,
                expression);
        }
    }
    // [Rn] case
    // only other case with only two arguments
    else if (instr->argSize == 2) {
        // +1 to ignore first square bracket in string
        rn = get_reg_num(args[1] + 1) << 16;
    } else {
        // checks whether this is the pre-indexed case or
        // post based on trailing ']'
		check_pre_index(args[1], &preIndexBit);
        rn = get_reg_num(args[1] + 1) << 16;
        // checks whether reg or immediate
        if (is_reg(args[2])) {
            immBit = IMM_BIT;
            if (check_negative_reg(args[2])) {
                upBit = 0;
                negOffset = 1;
            }
            offset = get_reg_num(args[2] + negOffset);
        } else {
            if (check_negative_imm(args[2])) {
                upBit = 0;
                negOffset = 1;
            }
            offset = get_immediate(args[2] + negOffset);
        }
        // if more than 3 args then it must be a shift
        if (instr->argSize > 3) {
            shift(&offset, args[3], args[4]);
        }
    }

    uint32_t result = AL_COND | SDT_CONST | immBit | preIndexBit | upBit |
        loadBit | rn | rd  | offset;
    set_instruction(curr->decoded, curr->pc, result);
}

void branch(instruction *instr, state *curr) {
    int32_t offset;
    uint32_t address;
    // to make code more readable
    char **args = instr->args;
    if (is_imm(args[0])) {
        offset = get_immediate(args[0]) - curr->pc - 8;
    } else {
        address = get_label_address(curr->labels, args[0], curr->labelCount);
        offset = address - curr->pc - 8;
    }
    offset >>= 2;
    offset &= 0xFFFFFF;
    uint32_t result = instr->u.condCode << SHIFT_COND | BRANCH_CONST | offset;
    set_instruction(curr->decoded, curr->pc, result);
}

void halt(instruction *instr, state *curr) {
    set_instruction(curr->decoded, curr->pc, 0);
}

void read_file_first(firstFile *firstRead, char *inputFileName) {
    FILE *file;
    file = fopen(inputFileName, "r");
    if (!file) {
        perror("File not found:");
		exit(EXIT_FAILURE);
        return;
    }
    int line = 0;
    int labelCount = 0;
    char str[MAX_LINE];
    while (fgets(str, MAX_LINE, file)) {
        if (str[strlen(str) - 2] == ':') {
            str[strlen(str) - 2] = '\0';
			firstRead->labels = realloc_labels(firstRead->labels, labelCount);
            strcpy(firstRead->labels[labelCount].s, str);
            firstRead->labels[labelCount].i = (line - labelCount) * 4;
            labelCount++;
        }
        if (*str != '\n') {
          line++;
        }

    }
    fclose(file);
    firstRead->lines = line - labelCount;
	firstRead->labelCount = labelCount;
}

void read_file_second(state *curr, char *inputFileName) {
    FILE *file;
    file = fopen(inputFileName, "r");
    if (!file) {
        perror("File not found:");
		exit(EXIT_FAILURE);
    }
    instruction *instr = initalise_instruction();
    char str[MAX_LINE];
    char *argsInInstruction;
	char *ptrToFirstSpace;
    while (fgets(str, MAX_LINE, file)) {
        ptrToFirstSpace = strchr(str, ' ');
		// will skip instruction if label as that is accounted for in branch
		// and first read
        if (!ptrToFirstSpace) {
            continue;
        }
        argsInInstruction = ptrToFirstSpace + 1;
        *ptrToFirstSpace = '\0';
        split_on_commas(strtok(argsInInstruction, "\n"), instr);
        int abstractData = mnemonic_key(str, instr);
        if (abstractData == -1) {
            fprintf(stderr, "Unsupported instruction\n");
            exit(EXIT_FAILURE);
        }
        abstract_type[abstractData](instr, curr);
        if (!curr->decoded[curr->pc / 4]) {
		    break;
		}
		curr->pc += 4;
    }
    fclose(file);
    free_instruction(instr);
}

void write_file(state *curr, char *fileName){
	FILE *binFile;
	binFile = fopen(fileName, "wb");
    if (!binFile) {
        perror("File not found:");
		exit(EXIT_FAILURE);
    }
	fwrite(curr->decoded, sizeof(uint32_t), curr->lastInstr, binFile);
	fclose(binFile);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Program does not have correct number of arguments.\n");
        exit(EXIT_FAILURE);
    }
    firstFile *firstRead = initalise_first_file();
    read_file_first(firstRead, argv[1]);

    state *curr = initalise_state(firstRead);
    read_file_second(curr, argv[1]);

    write_file(curr, argv[2]);

    free_state(curr, firstRead->labelCount);
    free(firstRead);
    return 0;
}
