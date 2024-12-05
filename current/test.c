#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <endian.h>

/*
void read_and_print_4byte_chunks(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    uint32_t buffer;
    size_t bytesRead;

    printf("Hexadecimal values (4-byte chunks, big-endian):\n");
    while ((bytesRead = fread(&buffer, 1, sizeof(buffer), file)) == sizeof(buffer)) {
        // Convert to big-endian if necessary using the htonl function
        uint32_t converted = htonl(buffer);
        printf("0x%08X\n", converted);
    }

    // Handle any leftover bytes (if the file size is not a multiple of 4 bytes)
    if (bytesRead > 0) {
        buffer = 0; // Zero out the buffer
        fread(&buffer, 1, bytesRead, file);
        uint32_t converted = htonl(buffer);
        printf("0x%08X\n", converted);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    read_and_print_4byte_chunks(argv[1]);

    return EXIT_SUCCESS;
}
*/

int main() {

    
    return 0;
}