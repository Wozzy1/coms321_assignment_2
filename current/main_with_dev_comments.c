// Wilson Diep diepw50@iastate.edu
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#define LINE_SIZE 35
#define NUM_INSTRUCTIONS 26
#define DEBUG 0

uint32_t to_LE(uint32_t value) {
    // if the system is little-endian, no conversion is needed
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return value;
#else
    // convert to little-endian if the system is big-endian
    return ((value >> 24) & 0x000000FF) |
           ((value >> 8)  & 0x0000FF00) |
           ((value << 8)  & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
#endif
}

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
void decode_instruction(char* line, uint32_t binary, int* branch_offset);
void printInstruction(instruction* inst);
unsigned int countBits(unsigned int n);
void appendBits(char* str, int start, unsigned int binary, int length);
void printBits(unsigned int n);
void replaceSpecialRegister(char* reg, int r);
void replaceBranchCondition(char* condition, int rt);
size_t count_lines(const char *filename);

/**
 * Given an opcode, linear search through instructions 
 * and return the index of which instruction matches.
 */
int findInstruction(int opcode) {
    // searching for R and D type
    for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
        if (opcode == instructions[i].opcode) {
            if (DEBUG) printf("opcode received: %#x\n", opcode);
            return i;
        }
    }
    // searching for I type
    for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
        if (((opcode >> 1) & 0x7FF) == instructions[i].opcode) {
            if (DEBUG) printf("opcode received: %#x\n", (opcode >> 1) & 0x7FF);
            return i;
        }
    }
    // searching for B type
    for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
        if (((opcode >> 5) & 0x3F) == instructions[i].opcode) {
            if (DEBUG) printf("opcode received: %#x\n", (opcode >> 5) & 0x3F);
            return i;
        }
    }
    // searching for CB type
    for (int i = 0; i <= NUM_INSTRUCTIONS; i++) {
        if (((opcode >> 3) & 0xFF) == instructions[i].opcode) {
            if (DEBUG) printf("opcode received: %#x\n", (opcode >> 3) & 0xFF);
            return i;
        }
    }

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
    if (DEBUG) printf("Received type: %s\n", type);

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
        if (opcode == 0b11111111101) {
            sprintf(line, "%-5s ", inst->mnemonic);
            if (28 <= rd && rd <= 31) {
                char rd_c[4];
                replaceSpecialRegister(rd_c, rd);
                strcat(line, rd_c);
            } else {
                char temp[5];
                sprintf(temp, "x%d", rd);
                strcat(line, temp);
            }
            return;
        }

        if (opcode == 0b11111111100 || opcode == 0b11111111110 || opcode == 0b11111111111) {
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
        if (DEBUG) printf("%X x%d x%d %d\n", opcode, rd, rn, immediate);

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

        if (DEBUG) printf("%X x%d [x%d, #%d]\n", opcode, rt, rn, dt_address);

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
void decode_instruction(char* line, uint32_t binary, int* branch_offset) {
    int opcode = (binary >> 21) & 0x7FF;
    int res = findInstruction(opcode);
    if (DEBUG) printf("index of instruction: %d\n", res);

    createLine(line, &instructions[res], binary, branch_offset);

    // printf("  %s\n", line);
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
 * Returns number of lines in the file to read.
 */
size_t count_lines(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    uint32_t buffer;
    size_t line_count = 0;
    size_t bytesRead;

    while ((bytesRead = fread(&buffer, 1, sizeof(buffer), file)) == sizeof(buffer)) {
        line_count++;
    }

    // Handle any leftover bytes (if the file size is not a multiple of 4 bytes)
    if (bytesRead > 0) {
        line_count++; // Counting the last chunk, even if it's not 4 bytes
    }

    fclose(file);
    return line_count;
}
/**
 * Count number of lines after disassembling 
 * create array of boolean assign 1 if "label_index:\n" needs to be displayed
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    size_t lineCount = 0;
    lineCount = count_lines(argv[1]); // if this doesn't error, next line shouldn't
    if (DEBUG) printf("%ld\n", lineCount);

    char **lines = malloc((lineCount+1) * sizeof(char *)); // +1
    int label_pos[lineCount+1];                            // +1
    int labelCount = 0;
    for (int i = 0; i < lineCount+1; i++) {                // +1
        lines[i] = malloc(LINE_SIZE * sizeof(char)); // allocate space for each line
        label_pos[i] = 0;
    }
    int pos = 0;

    FILE *file = fopen(argv[1], "rb");
    uint32_t buffer;
    size_t bytesRead;

    printf("Processing instructions...\n");
    while ((bytesRead = fread(&buffer, 1, sizeof(buffer), file)) == sizeof(buffer)) {
        // Convert to big-endian if necessary using the htonl function
        uint32_t converted = htonl(buffer);
        if (DEBUG) printf("0x%08X\n", converted);
        char line[LINE_SIZE];
        int offset = 0;
        int* offset_p = &offset;
        decode_instruction(line, converted, offset_p);
        strcpy(lines[pos++], line);
        
        if (offset != 0) { // end case of branching 0 lines, but ignoring that
            label_pos[pos + offset] = 1; // place label flag
        }
    }

    if (!feof(file)) {
        perror("Error reading file");
        fclose(file);
        return EXIT_FAILURE;
    }

    fclose(file);
    // for (int i = 0; i <= lineCount; i++) {
    //     printf("%d %s\n", label_pos[i], lines[i]);
    // }
    // printf("\n\n");
    /**
     * 1) place label flag first, 
     * 2) go through label_pos, if 1, add label_#\n to lines[pos]
     * then when going through all instructions,
     *   if instruction is branch type, 
     *     then go to the offset and read lines[pos + offset] until '\n' 
     *     then copy until '\n', use to replace lines[pos]'s #offset
     */
    for (int i = 0; i < lineCount+1; i++) { // +1
        if (label_pos[i] != 0) {
            char newLine[LINE_SIZE];
            sprintf(newLine, "label_%d:\n%s", ++labelCount, lines[i-1]);
            strcpy(lines[i-1], newLine);
            // printf("--> %s\n", lines[i-1]);
            // printf("--> %s\n", newLine);
        }
    }

    // for (int i = 0; i < lineCount+1; i++) {
    //     printf("%d %s\n", label_pos[i], lines[i]);
    // }
    
    for (int i = 0; i < lineCount+1; i++) { // +1
        // if B or B.cond or BL
        if (lines[i][0] == 'B' && (lines[i][1] == '.' || lines[i][1] == ' ' || lines[i][1] == 'L')) {
            int index = 6;
            pos = 0;
            char nums_c[15]; // arbitrary 15, hopefully no programs with label number overflowing
            while (index < strlen(lines[i]) && lines[i][index]) {
                nums_c[pos++] = lines[i][index++];
            }
            nums_c[pos] = '\0';
            int offset = atoi(nums_c); // convert the offset in instruction to int
            // printf("--> %s\n", lines[i + offset]);
            
            // use offset to find correct line, then copy label
            index = 0;
            char label_to_copy[15];
            pos = 0;
            while (lines[i + offset][index] != ':') {
                // printf("%c", lines[i + offset][index]);
                label_to_copy[pos++] = lines[i + offset][index++];
            }
            label_to_copy[pos] = '\0';
            // printf("----> %s\n", label_to_copy);
            strcpy(lines[i]+5, label_to_copy);
        }
        else if ((lines[i][0] == 'C' && lines[i][1] == 'B')) { // if inst starts with "CB" => CBZ & CBNZ
            // printf("--> %s\n", lines[i]);
            int index = 4;
            // find index of '#' could have variable length operands
            while (index < strlen(lines[i]) && lines[i][index] != '#') {
                index++;
            }
            int start_index = index;
            while (index < strlen(lines[i]) && lines[i][index] != '\0') {
                index++;
            }
            int end_index = index;

            char nums_c[15]; // arbitrary array size
            pos = 0;
            while (start_index < end_index) {
                // printf("%c", lines[i][1 + start_index]);
                nums_c[pos++] = lines[i][1 + start_index++];
            }
            int offset = atoi(nums_c);
            // printf("num: %d\n", offset);
            // printf("%s\n", nums_c);

            // use offset to find correct line, then copy label
            index = 0;
            char label_to_copy[15];
            pos = 0;
            printf("The culprit --> %s\n", lines[i+offset]);
            while (lines[i + offset][index] != ':') {
                printf("%c", lines[i + offset][index]);
                label_to_copy[pos++] = lines[i + offset][index++];
            }
            label_to_copy[pos] = '\0';
            
            // find the position to insert the label name to branch to
            index = 0;
            while (index < strlen(lines[i]) && lines[i][index] != ',') {
                index++;
            }
            index += 2;
            strcpy(lines[i] + index, label_to_copy);
        }
    }

    /**
     * Clean up for nested branches:
     *   iterate through all lines that have a label,
     *   check if it is a branch inst
     *     iterate past label, then check for B or C start
     *   check if there is a '#' in the string
     *   if so, go to that line and replace it with the correct label
     */

    for (int i = 0; i < lineCount; i++) {
        if (lines[i][0] == 'l') {
            // printf("TESTING %s\n", lines[i]);
            int index = 8; // "label_1:\n" minimum length = 8
            // index of hashtag
            int hashIndex;
            
            // iterate until '#' or end of string
            while (index < strlen(lines[i]) && lines[i][index] != '#') {
                index++;
            }

            // if no '#' then continue
            if (index >= strlen(lines[i])) {
                continue;
            }
            hashIndex = index; // store the index of '#'

            // now filter by lines that dont have "\nB" or "\nC"
            index = 0;
            while (index < strlen(lines[i]) && lines[i][index] != ':') {
                index++;
            }
            if (lines[i][index+2] != 'B' && lines[i][index+2] != 'C') {
                // printf("TESTING %s\n", lines[i]+index);
                continue;
            }
            // while (index < strlen(lines[i]) && (lines[i][index-1] != '\n' && (lines[i][index] != 'B' || lines[i][index] != 'C'))) {
            //     index++;
            // } // leaves loop with index of first character if it is "\nB" or "\nC"
            // 
            // if (index >= strlen(lines[i])) {
            //     continue;
            // }
            // now we know it is labeled inst + B or CB


            // copies the immediate to nums_c
            char nums_c[15];
            pos = 0;
            int tempIndex = hashIndex + 1;
            while (lines[i][tempIndex] != '\0') {
                nums_c[pos++] = lines[i][tempIndex++];
            }
            nums_c[pos] = '\0';
            // printf("%s\n", nums_c);
            int offset = atoi(nums_c);
            // printf("%d\n", offset);
            // printf("--> %s\n", lines[i]);
            // printf("--> %s\n", lines[i+offset]);

            index = 0;
            char label_to_copy[15];
            pos = 0;
            while (lines[i + offset][index] != ':') {
                // printf("%c", lines[i + offset][index]);
                label_to_copy[pos++] = lines[i + offset][index++];
            }
            label_to_copy[pos] = '\0';
            strcpy(lines[i]+hashIndex, label_to_copy);

        }
    }

    printf("\nConverted:\n");
    for (int i = 0; i < lineCount; i++) {
        printf("%s\n", lines[i]);
        // printf("%ld\n", strlen(lines[i]));
        // free(lines[i]);
    }
    // free(lines);

    
    printf("All instructions processed.\n");
    return EXIT_SUCCESS;
}