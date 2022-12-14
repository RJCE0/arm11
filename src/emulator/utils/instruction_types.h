#ifndef INSTRUCTION_TYPES
#define INSTRUCTION_TYPES

typedef struct {
    instructionOpcode opcode: 4;
    uint16_t operand2: 12;
    uint8_t rn: 4;
    uint8_t rd: 4;

    bool immediate;
    bool setBit;
} dataProcessingInstruction;

typedef struct {
    uint8_t rn: 4;
    uint8_t rd: 4;
    uint8_t rs: 4;
    uint8_t rm: 4;

    bool accumBit;
    bool setBit;
} multiplyInstruction;

typedef struct {
    uint16_t rn: 4;
    uint16_t rd: 4;
    uint32_t offset: 12;

    bool upBit;
    bool loadBit;
    bool indexingBit;
    bool immediate;
} sdtInstruction;

typedef struct {
    uint32_t offset;
} branchInstruction;

typedef struct {
    instructionType type: 3;
    uint8_t condCode: 4;
    union {
        dataProcessingInstruction dpi;
        multiplyInstruction mi;
        sdtInstruction sdti;
        branchInstruction bi;
    } u;
} decodedInstruction;

#endif
