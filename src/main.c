#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// TODO: This assumes little-endianness. Not good.
typedef struct {
    uint32_t magic;
    uint8_t bits_32_or_64;
    uint8_t endianness;
    uint8_t version;
    uint8_t os_abi;
    uint8_t os_abi_version;
    uint8_t padding[7];
    uint16_t file_type;
    uint16_t machine;
    uint32_t e_version;
    uint64_t entry;
    uint64_t ph_off;
    uint64_t sh_off;
    uint32_t eflags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrindx;
} ElfHeader;

typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
} ProgramHeader;

typedef struct {
    uint32_t name;
    uint32_t type;
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;
} SectionHeader;

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

    ElfHeader elf_header = *(ElfHeader*)cursor;
    assert(sizeof(ProgramHeader) == elf_header.phentsize);
    assert(sizeof(SectionHeader) == elf_header.shentsize);
    cursor += sizeof(elf_header);

    if (elf_header.bits_32_or_64 != 2) {
        fprintf(stderr, "ERROR: Exe is not 64 bits. This program assumes 64 bits\n");
        return 1;
    }
    const unsigned int bits = 64;

    if (elf_header.endianness != 1) {
        fprintf(stderr, "ERROR: Big endian file found. This program assumes little endianness\n");
        return 1;
    }

    printf("================================\n");
    printf("* Header:\n");
    printf("================================\n");
    printf("INFO: Magic = 0x%X\n", elf_header.magic);
    printf("INFO: Class = %d\n", elf_header.bits_32_or_64);
    printf("\tTherefore bits = %d\n", bits);
    printf("INFO: Endianness = %d\n", elf_header.endianness);
    printf("INFO: Version = %d\n", elf_header.version);
    printf("INFO: OS ABI = %d\n", elf_header.os_abi);
    printf("INFO: OS ABI Version = %d\n", elf_header.os_abi_version);
    printf("INFO: File Type = 0x%X\n", elf_header.file_type);
    printf("INFO: Machine = 0x%X\n", elf_header.machine);
    printf("INFO: e_version = 0x%X\n", elf_header.e_version);
    printf("INFO: entry = 0x%lX\n", elf_header.entry);
    printf("INFO: ph_off = 0x%lX\n", elf_header.ph_off);
    printf("INFO: sh_off = 0x%lX\n", elf_header.sh_off);
    printf("INFO: eflags = 0x%X\n", elf_header.eflags);
    printf("INFO: ehsize = 0x%X\n", elf_header.ehsize);
    printf("INFO: phentsize = 0x%X\n", elf_header.phentsize);
    printf("\tsizeof(ProgramHeader) = 0x%lX\n", sizeof(ProgramHeader));
    printf("INFO: phnum = %d\n", elf_header.phnum);
    printf("INFO: shentsize = 0x%X\n", elf_header.shentsize);
    printf("INFO: shnum = %d\n", elf_header.shnum);
    printf("INFO: shstrindx = 0x%X\n", elf_header.shstrindx);
    printf("================================\n");
    printf("Header size: 0x%lX\n", sizeof(elf_header));
    printf("================================\n");

    cursor = buffer + elf_header.ph_off;
    ProgramHeader *p_header = malloc(elf_header.phnum * sizeof(ProgramHeader));
    if (p_header == NULL) {
        fprintf(stderr, "ERROR: Buy more RAM\n");
        return 1;
    }
    memcpy(p_header, cursor, elf_header.phnum * sizeof(ProgramHeader));

    cursor = buffer + elf_header.sh_off;
    assert(sizeof(SectionHeader) == elf_header.shentsize);
    SectionHeader *s_header = malloc(elf_header.shnum * sizeof(SectionHeader));
    if (s_header == NULL) {
        fprintf(stderr, "ERROR: Buy more RAM\n");
        return 1;
    }
    memcpy(s_header, cursor, elf_header.shnum * sizeof(SectionHeader));

    for (size_t i=0; i<elf_header.phnum; i++) {
        printf("Segment #%zu:\n", i);
        printf("\tSize on disk = %lu bytes\n", p_header[i].filesz);
        printf("\tFlags = %d\n", p_header[i].flags);
        printf("\tLocation in file = 0x%lX\n", p_header[i].offset);
    }

    free(s_header);
    free(p_header);
    free(buffer);
    fclose(tgt_file);

    return 0;
}
