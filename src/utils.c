#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Fatal Error: Memory allocation failed for %lu bytes.\n", (unsigned long)size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void safe_strncpy(char* dest, const char* src, size_t dest_size) {
    if (dest_size == 0 || dest == NULL || src == NULL) return;
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}
