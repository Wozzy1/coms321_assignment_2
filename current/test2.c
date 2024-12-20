#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "rb");
    uint32_t buffer;
    size_t bytesRead;
    printf("test\n");
    while ((bytesRead = fread(&buffer, 1, sizeof(buffer), file)) == sizeof(buffer)) {
        // convert to big-endian if necessary using the htonl function
        uint32_t converted = htonl(buffer);
        printf("0x%08X\n", converted);
            
    }

    fclose(file);
    return 0;
}