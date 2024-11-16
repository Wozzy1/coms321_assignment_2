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

int findInstruction(int opcode);
void createLine(char* line, instruction* inst, int binary);
void decode_instruction(int binary);
void printInstruction(instruction* inst);
unsigned int countBits(unsigned int n);
void appendBits(char* str, int start, unsigned int binary, int length);
void printBits(unsigned int n);
void replaceSpecialRegister(char* reg, int r);

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
    printf("Received type: %s\n", type);

    // find the fields for each type and create the line to be sent back
    if (!strcmp(type, "R")) {
        int rd = binary & 0x1F;
        int rn = (binary >> 5) & 0x1F;
        int shamt = (binary >> 10) & 0x3F;
        int rm = (binary >> 16) & 0x1F;
        int opcode = (binary >> 21) & 0x7FF;
        printf("%X x%d %X %X %X\n", opcode, rm, shamt, rn, rd);

        // need to check if the register number is special ie, SP, FP, LR, XZR (28:31)

        // if LSL or LSR, then shamt matters
        /*
        LSL x1, x1, #3
        */



        sprintf(line, "%-5s ", inst->mnemonic);
        // check if rd is special or not
        if (28 <= rd && rd <= 31) {
            char rd_c[4];
            replaceSpecialRegister(rd_c, rd);
            // sprintf(line, "%s ", rd_c);
            strcat(line, rd_c);
            strcat(line, ", ");
        } else {
            char temp[5];
            sprintf(temp, "x%d, ", rd);
            strcat(line, temp);
        }
        // check if rn is special or not
        if (28 <= rn && rn <= 31) {
            char rn_c[4];
            replaceSpecialRegister(rn_c, rn);
            // sprintf(line, "%s ", rn_c);
            strcat(line, rn_c);
            strcat(line, ", ");
        } else {
            char temp[5];
            sprintf(temp, "x%d, ", rn);
            strcat(line, temp);
        }

        if (!strcmp(inst->mnemonic, "LSL") || !strcmp(inst->mnemonic, "LSR")) {
            char temp[5];
            sprintf(temp, "#%d ", shamt);
            strcat(line, temp);
        } else { // cases for not LSR or LSR
            // check if rm is special or not
            if (28 <= rm && rm <= 31) {
                char rm_c[4];
                replaceSpecialRegister(rm_c, rm);
                // sprintf(line, "%s ", rm_c);
                strcat(line, rm_c);
                strcat(line, " ");
            } else {
                char temp[5];
                sprintf(temp, "x%d ", rm);
                strcat(line, temp);
            }
        }

        // sprintf(line, "%-5s x%d, x%d, x%d", inst->mnemonic, rd, rn, rm);

        // appendBits(line, 0, opcode, 11);

    } else if (!strcmp(type, "I")) {

    } else if (!strcmp(type, "D")) {

    } else if (!strcmp(type, "B")) {

    } else if (!strcmp(type, "CB")) {

    }

}

void decode_instruction(int binary) {
    int opcode = (binary >> 21) & 0x7FF;
    int res = findInstruction(opcode);
    printf("index of instruction: %d\n", res);

    char line[50];
    createLine(line, &instructions[res], binary);

    printf("%s\n", line);
    
}


// update later to handle actual printing, like name and fields
void printInstruction(instruction* inst) {
    printf("%-5s %s %#x\n", inst->mnemonic, inst->type, inst->opcode);
}

unsigned int countBits(unsigned int n) 
{ 
   unsigned int count = 0; 
   while (n) { 
        count++; 
        n >>= 1; 
    } 
    return count; 
}

void appendBits(char* str, int start, unsigned int binary, int length) {
    for (int i = start, pos = 0; i < length; i++, pos++) {
        str[i] = (binary >> pos) & 0x1 ? '1' : '0';
    }
}

void printBits(unsigned int n) {
    int numBits = countBits(n);
    // char* ptr = malloc(ptr, numBits * 1);
    for (int i = 0; i < numBits; i++) {
        printf("%d", (n >>= i) & 0x1); 
    }
    printf("\n");
}

void replaceSpecialRegister(char* reg, int r) {
    switch (r) {
    case 28:                
        strcpy(reg, "SP");
        break;
    case 29:
        strcpy(reg, "FP");
        break;
    case 30:
        strcpy(reg, "LR");
        break;
    case 31:
        strcpy(reg, "XZR");
        break;
    };

}

int main() {
    // SUB X12, X10, XZR = 11001011000111110000000101001100

    // printInstruction(&instructions[0]);
    // decode_instruction(0b11001011000111110000000101001100);
    decode_instruction(0x9B030041);
    // printf("Count %u\n", countBits(0b10001011000111110000001111100000));
    // printf("%lu", sizeof(int));

    // char x[5];
    // appendBits(x, 0, 0b11011, 5);
    // printf("%s\n", x);
    return 0;
}