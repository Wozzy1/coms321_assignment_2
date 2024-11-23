#include<stdio.h>
#include<string.h>
#define NUM_INSTRUCTIONS 26

/**
 * Custom struct type for a given instruction that 
 * has a mnemonic, an instruction type, and opcode.
 */
typedef struct instruction {
    char mnemonic[6];   // 6 bytes
    char type[3];       // 3 bytes
    short opcode;       // 2 bytes
} instruction;

/**
 * Small look up table of the needed instructions, 
 * the instruction type, and the opcode in binary.
 */
instruction instructions[] = {
    {"ADD",   "R",  0b10001011000},
    {"ADDI",  "I",  0b1001000100},
    {"AND",   "R",  0b10001010000},
    {"ANDI",  "I",  0b1001001000},
    {"B",     "B",  0b000101},
    {"BL",    "B",  0b100101},
    {"B.",    "CB", 0b01010100},
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
void createLine(char* line, instruction* inst, int binary, int* branch_offset);
void decode_instruction(int binary);
void printInstruction(instruction* inst);
unsigned int countBits(unsigned int n);
void appendBits(char* str, int start, unsigned int binary, int length);
void printBits(unsigned int n);
void replaceSpecialRegister(char* reg, int r);
void replaceBranchCondition(char* condition, int rt);

/**
 * Given an opcode, linear search through instructions 
 * and return the index of which instruction matches.
 */
int findInstruction(int opcode) {
    // searching for R and D type
    for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
        if (opcode == instructions[i].opcode) {
            printf("opcode received: %#x\n", opcode);
            return i;
        }
    }
    // searching for I type
    for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
        if (((opcode >> 1) & 0x7FF) == instructions[i].opcode) {
            printf("opcode received: %#x\n", (opcode >> 1) & 0x7FF);
            return i;
        }
    }
    // searching for B type
    for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
        if (((opcode >> 5) & 0x3F) == instructions[i].opcode) {
            printf("opcode received: %#x\n", (opcode >> 5) & 0x3F);
            return i;
        }
    }
    // searching for CB type
    for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
        if (((opcode >> 3) & 0xFF) == instructions[i].opcode) {
            printf("opcode received: %#x\n", (opcode >> 3) & 0xFF);
            return i;
        }
    }

    
    // for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
    //     if (opcode == instructions[i].opcode) {
    //         printf("opcode received: %#x\n", opcode);
    //         return i;
    //     }
    // }
    // for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
    //     if (opcode == instructions[i].opcode) {
    //         printf("opcode received: %#x\n", opcode);
    //         return i;
    //     }
    // }

    return -1;
}

/**
 * Core method that requires knowing what instruction the 32 bit string represents,
 * then parses it, formats it into assembly, and returns the formatted string.
 * 
 * Handles the determination of the instruction type and acts accordingly.
 */
void createLine(char* line, instruction* inst, int binary, int* branch_offset) {
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
        // printf("%X x%d %X %X %X\n", opcode, rm, shamt, rn, rd);

        // need to check if the register number is special ie, SP, FP, LR, XZR (28:31)

        // if PRNT, PRNL, DUMP, HALT then print only the instruction name
        if (opcode == 0b11111111101 || opcode == 0b11111111100 || opcode == 0b11111111110 || opcode == 0b11111111111) {
            sprintf(line, "%-5s ", inst->mnemonic);
            return;
        }

        sprintf(line, "%-5s ", inst->mnemonic);
        if (opcode == 0x6B0) { // BR
            if (28 <= rn && rn <= 31) {
                char rn_c[4];
                replaceSpecialRegister(rn_c, rn);
                // sprintf(line, "%s ", rn_c);
                strcat(line, rn_c);
            } else {
                char temp[5];
                sprintf(temp, "x%d ", rn);
                strcat(line, temp);
            }
            return;
        }

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

        // check for LSR or LSR since they do not use RM, instead use SHAMT
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
    } else if (!strcmp(type, "I")) {
        int rd = binary & 0x1F;
        int rn = (binary >> 5) & 0x1F;
        int immediate = (binary >> 10) & 0xFFF;
        int opcode = (binary >> 22) & 0x3FF;
        printf("%X x%d x%d %d\n", opcode, rd, rn, immediate);

        sprintf(line, "%-6s", inst->mnemonic);
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

        char immediate_c[7];
        sprintf(immediate_c, "#%d", immediate);
        strcat(line, immediate_c);

    } else if (!strcmp(type, "D")) {
        int rt = binary & 0x1F;
        int rn = (binary >> 5) & 0x1F;
        // int op = (binary >> 10) & 0x3; // unused two bits
        int dt_address = (binary >> 12) & 0x1FF;
        int opcode = (binary >> 21) & 0x7FF;

        printf("%X x%d [x%d, #%d]\n", opcode, rt, rn, dt_address);

        sprintf(line, "%-5s", inst->mnemonic);

        if (28 <= rt && rt <= 31) {
            char rt_c[4];
            replaceSpecialRegister(rt_c, rt);
            strcat(line, rt_c);
            strcat(line, ", [");
        } else {
            char temp[8];
            sprintf(temp, "x%d, [", rt);
            strcat(line, temp);
        }
        // check if rn is special or not
        if (28 <= rn && rn <= 31) {
            char rn_c[6];
            replaceSpecialRegister(rn_c, rn);
            // sprintf(line, "%s ", rn_c);
            strcat(line, rn_c);
            strcat(line, ", ");
        } else {
            char temp[6];
            sprintf(temp, "x%d, ", rn);
            strcat(line, temp);
        }

        char offset[6];
        sprintf(offset, "#%d]", dt_address);
        strcat(line, offset);
    } else if (!strcmp(type, "B")) {
        signed int br_address = binary & 0x3FFFF;
        // int opcode = (binary >> 26) & 0x3F;

        if (br_address & (1 << 17)) { // check if the 18th bit is 1
            br_address |= ~0x3FFFF;   // sign extend to 32 bits
        }
        // need to handle labeling towards the end
        sprintf(line, "%-5s", inst->mnemonic);
        char offset[10];
        sprintf(offset, "#%d", br_address);
        strcat(line, offset);
        (*branch_offset) = br_address;
        // TODO return br_address through pointer variable for later showing labels

    } else if (!strcmp(type, "CB")) {
        int rt = binary & 0x1F;
        signed int cond_br_address = (binary >> 5) & 0x7FFFF;
        int opcode = (binary >> 24) & 0xFF;
        if (cond_br_address & (1 << 18)) {
            cond_br_address |= ~0x7FFFF;
        }

        // have to handle B.cond before the below
        if (opcode == 0x54) {   // B.cond
            char cond[3];
            replaceBranchCondition(cond, rt);
            sprintf(line, "B.%-3s#%d", cond, cond_br_address);
            (*branch_offset) = cond_br_address;
            return;
        } 


        sprintf(line, "%-5s", inst->mnemonic);
        char offset[10];
        if (28 <= rt && rt <= 31) {
            char rt_c[4];
            replaceSpecialRegister(rt_c, rt);
            strcat(line, rt_c);
            strcat(line, ", ");
        } else {
            char temp[5];
            sprintf(temp, "x%d, ", rt);
            strcat(line, temp);
        }
        sprintf(offset, "#%d", cond_br_address);
        strcat(line, offset);
        (*branch_offset) = cond_br_address;
    }

}

/**
 * Core method that takes a 32 bit instruction, disassembles it,
 * and prints it to the console (for now).
 * TODO update method to take a pointer of array of all dissassembled
 * instructions.
 */
void decode_instruction(int binary) {
    int opcode = (binary >> 21) & 0x7FF;
    int res = findInstruction(opcode);
    printf("index of instruction: %d\n", res);

    int offset;
    int* branch_offset = &offset;
    char line[25];
    createLine(line, &instructions[res], binary, branch_offset);

    printf("  %s\n", line);
    
}


// update later to handle actual printing, like name and fields
void printInstruction(instruction* inst) {
    printf("%-5s %s %#x\n", inst->mnemonic, inst->type, inst->opcode);
}

/**
 * Helper method to return the number of bits in a bitstring
 */
unsigned int countBits(unsigned int n) 
{ 
   unsigned int count = 0; 
   while (n) { 
        count++; 
        n >>= 1; 
    } 
    return count; 
}

/**
 * Helper method to concat a binary value to a string
 */
void appendBits(char* str, int start, unsigned int binary, int length) {
    for (int i = start, pos = 0; i < length; i++, pos++) {
        str[i] = (binary >> pos) & 0x1 ? '1' : '0';
    }
}

/**
 * Helper method to print a bianry string of any length
 */
void printBits(unsigned int n) {
    int numBits = countBits(n);
    // char* ptr = malloc(ptr, numBits * 1);
    for (int i = 0; i < numBits; i++) {
        printf("%d", (n >>= i) & 0x1); 
    }
    printf("\n");
}

/**
 * Helper method to handle special register names 
 * such as SP, FP, LR, and XZR when printing.
 */
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

/**
 * Given the Rt of the B.cond instruction, writes
 * the appropriate condition string to char* condition.
 */
void replaceBranchCondition(char* condition, int rt) {
    switch (rt) {
        case 0x0:
            strcpy(condition, "EQ");
            break;
        case 0x1:
            strcpy(condition, "NE");
            break;
        case 0x2:
            strcpy(condition, "HS");
            break;
        case 0x3:
            strcpy(condition, "LO");
            break;
        case 0x4:
            strcpy(condition, "MI");
            break;
        case 0x5:
            strcpy(condition, "PL");
            break;
        case 0x6:
            strcpy(condition, "VS");
            break;
        case 0x7:
            strcpy(condition, "VC");
            break;
        case 0x8:
            strcpy(condition, "HI");
            break;
        case 0x9:
            strcpy(condition, "LS");
            break;
        case 0xA:
            strcpy(condition, "GE");
            break;
        case 0xB:
            strcpy(condition, "LT");
            break;
        case 0xC:
            strcpy(condition, "GT");
            break;
        case 0xD:
            strcpy(condition, "LE");
            break;
        default:
            printf("ERROR in replaceBranchCondition with input: %d\n", rt);
    }
}


/**
 * Count number of lines after disassembling 
 * create array of boolean assign 1 if "label_index:\n" needs to be displayed
 */

int main() {
    // SUB X12, X10, XZR = 11001011000111110000000101001100

    // printInstruction(&instructions[0]);
    // decode_instruction(0b11001011000111110000000101001100);
    
//     int nums[] = { 
// 0x91002BE1,
// 0x92002822,
// 0xB20003E3,
// 0xD2000441,
// 0xD1002022,
// 0xF1003C24
//     };

    // for (int i = 0; i < 6; i++) {
    //     decode_instruction(nums[i]);
    // }
    // decode_instruction(0xB5FFFFA1);
    // decode_instruction(0xB4000021);
    decode_instruction(0x54FFFF64);
    // printf("Count %u\n", countBits(0b10001011000111110000001111100000));
    // printf("%lu", sizeof(int));

    // char x[5];
    // appendBits(x, 0, 0b11011, 5);
    // printf("%s\n", x);
    return 0;
}