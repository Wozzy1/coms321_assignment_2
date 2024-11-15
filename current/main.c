#include<stdio.h>
#include<string.h>
// #include"BinaryToASM.h"
#define NUM_INSTRUCTIONS 25

typedef struct instruction {
    char mnemonic[6];   // 6 bytes
    char type[3];       // 3 bytes
    short opcode;       // 2 bytes
} instruction;

// 25 instructions
instruction instructions[] = {
    {"ADD",   "R",  0b10001011000},
    {"ADDI",  "I",  0b1001000100},
    {"AND",   "R",  0b10001010000},
    {"ANDI",  "I",  0b1001001000},
    {"B",     "B",  0b000101},
    {"BL",    "B",  0b100101},
    {"BR",    "R",  0b11010110000},
    {"CBNZ",  "CB", 0b10110101},
    {"CBZ",   "CB", 0b10110100},
    {"EOR",   "R",  0b11001010000},
    {"EORI",  "I",  0b1101001000},
    {"LDUR",  "D",  0b11111000010},
    {"LSL",   "R",  0b11010011011},
    {"LSR",   "R",  0b11010011010},
    {"ORR",   "R",  0b10101010000},
    {"ORRI",  "I",  0b1011001000},
    {"STUR",  "D",  0b11111000000},
    {"SUB",   "R",  0b11001011000},
    {"SUBI",  "I",  0b1101000100},
    {"SUBIS", "I",  0b1111000100},
    {"SUBS",  "R",  0b11101011000},
    {"MUL",   "R",  0b10011011000},
    {"PRNT",  "R",  0b11111111101},
    {"PRNL",  "R",  0b11111111100},
    {"DUMP",  "R",  0b11111111110},
    {"HALT",  "R",  0b11111111111}
};

/**
 * Given an opcode, linear search through instructions 
 * and return the index of which instruction matches.
 */
int findInstruction(int opcode) {
    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (opcode == instructions[i].opcode) {
            printf("opcode received: %#x\n", opcode);
            return i;
        }
    }

    return -1;
}

void createLine(char* line, instruction* inst, int binary) {
    char type[3];
    strcpy(type, inst->type);

    // find the fields for each type and create the line to be sent back
    if (!strcmp(type, "R")) {

    } else if (!strcmp(type, "I")) {

    } else if (!strcmp(type, "D")) {

    } else if (!strcmp(type, "B")) {

    } else if (!strcmp(type, "CB")) {

    }

}

void decode_instruction(int binary) {
    int opcode = (binary >> 21) & 0x7FF;
    int res = findInstruction(opcode);
    printf("%d\n", res);

    char line[25];
    createLine(line, &instructions[res], binary);
    
}


// update later to handle actual printing, like name and fields
void printInstruction(instruction* inst) {
    printf("%-5s %c %#x", inst->mnemonic, inst->type, inst->opcode);
}

int main() {
    // SUB X12, X10, XZR = 11001011000111110000000101001100

    // printInstruction(&instructions[0]);
    decode_instruction(0b11001011000111110000000101001100);
    // printf("%lu", sizeof(int));
    return 0;
}