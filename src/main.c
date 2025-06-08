#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

typedef enum {
    ENDIAN_LITTLE,
    ENDIAN_BIG,
} Endianness;

// TODO: All these functions assumed little-endianness. Not good.
uint8_t get_u8(char **cursor) {
    uint8_t retval = *(uint8_t*)*cursor;
    *cursor += 1;
    return retval;
}

uint16_t get_u16(char **cursor) {
    uint16_t retval = *(uint16_t*)*cursor;
    *cursor += 2;
    return retval;
}

uint32_t get_u32(char **cursor) {
    uint32_t retval = *(uint32_t*)*cursor;
    *cursor += 4;
    return retval;
}

uint64_t get_u64(char **cursor) {
    uint64_t retval = *(uint64_t*)*cursor;
    *cursor += 8;
    return retval;
}

int main(void) {
    const char *target_path = "./testfiles/test_target";

    FILE *tgt_file = fopen(target_path, "r");
    if (tgt_file == NULL) {
        fprintf(stderr, "ERROR: Could not open file %s: %s\n", 
                target_path, 
                strerror(errno));
        return 1;
    }

    fseek(tgt_file, 0, SEEK_END);
    size_t file_size = (size_t)ftell(tgt_file);
    fseek(tgt_file, 0, SEEK_SET);

    printf("INFO: File %s has %zu bytes\n", target_path, file_size);

    char *buffer = malloc(file_size);
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: Buy more RAM\n");
        return 1;
    }

    size_t bytes_read = 0;
    while (bytes_read < file_size)
        bytes_read += fread(buffer + bytes_read, 1, file_size, tgt_file);
    assert(bytes_read == file_size);

    printf("INFO: Read %zu bytes from %s\n", bytes_read, target_path);

    char *cursor = buffer;

    uint32_t magic = get_u32(&cursor);

    unsigned int bits;
    uint8_t bits_32_or_64 = get_u8(&cursor);
    if (bits_32_or_64 == 1) bits = 32;
    else if (bits_32_or_64 == 2) bits = 64;
    else {
        fprintf(stderr, "ERROR: Could not interpret the bits field of the header (%d). Something is corrupted.\n", bits_32_or_64);
        return 1;
    }

    uint8_t endianness = get_u8(&cursor);
    if (endianness != 1) {
        fprintf(stderr, "ERROR: Big endian file found. This exe assumes little endianness\n");
        return 1;
    }

    uint8_t version = get_u8(&cursor);
    uint8_t os_abi = get_u8(&cursor);
    uint8_t os_abi_version = get_u8(&cursor);
    
    cursor += 7;  // Padding field

    uint16_t file_type = get_u16(&cursor);

    printf("INFO: Magic = 0x%x\n", magic);
    printf("INFO: Class = %d\n", bits_32_or_64);
    printf("\tTherefore bits = %d\n", bits);
    printf("INFO: Endianness = %d\n", endianness);
    printf("INFO: Version = %d\n", version);
    printf("INFO: OS ABI = %d\n", os_abi);
    printf("INFO: OS ABI Version = %d\n", os_abi_version);
    printf("INFO: File Type = 0x%04X", file_type);

    fclose(tgt_file);
    free(buffer);

    return 0;
}
