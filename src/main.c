#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    size_t file_size = ftell(tgt_file);
    fseek(tgt_file, 0, SEEK_SET);

    printf("INFO: File %s has %zu bytes\n", target_path, file_size);

    char *buffer = malloc(file_size);
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: Buy more RAM\n");
        return 1;
    }

    size_t bytes_read = fread(buffer, 1, file_size, tgt_file);
    assert(bytes_read == file_size);

    printf("INFO: Read %zu bytes from %s\n", bytes_read, target_path);

    fclose(tgt_file);
    free(buffer);

    return 0;
}
