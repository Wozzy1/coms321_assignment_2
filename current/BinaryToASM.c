#include<stdio.h>

typedef struct instruction {
    char mnemonic[6];   // 6 bytes
    char type;          // 1 byte
    short opcode;       // 2 bytes
} instruction;

// 25 instructions
instruction instructions[] = {
    {"ADD",   'R', 0b10001011000},
    {"ADDI",  'I', 0b1001000100},
    {"AND",   'R', 0b10001010000},
    {"ANDI",  'I', 0b1001001000},
    {"B",     'B', 0b000101},
    {"BL",    'B', 0b100101},
    {"BR",    'R', 0b11010110000},
    {"CBNZ",  'C', 0b10110101},
    {"CBZ",   'C', 0b10110100},
    {"EOR",   'R', 0b11001010000},
    {"EORI",  'I', 0b1101001000},
    {"LDUR",  'M', 0b11111000010},
    {"LSL",   'R', 0b11010011011},
    {"LSR",   'R', 0b11010011010},
    {"ORR",   'R', 0b10101010000},
    {"ORRI",  'I', 0b1011001000},
    {"STUR",  'M', 0b11111000000},
    {"SUB",   'R', 0b11001011000},
    {"SUBI",  'I', 0b1101000100},
    {"SUBIS", 'I', 0b1111000100},
    {"SUBS",  'R', 0b11101011000},
    {"MUL",   'R', 0b10011011000},
    {"PRNT",  'R', 0b11111111101},
    {"PRNL",  'R', 0b11111111100},
    {"DUMP",  'R', 0b11111111110},
    {"HALT",  'R', 0b11111111111}
};


void determine_instruction_type(int binary) {
    printf("%s %c %#x", instructions[0].mnemonic, instructions[0].type, instructions[0].opcode);

}