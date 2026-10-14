#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

/**
 * Safe memory allocation wrapper.
 * Terminates the program cleanly if allocation fails.
 */
void* safe_malloc(size_t size);

/**
 * Safe string copy, ensuring null-termination.
 * Prevents buffer overflows.
 */
void safe_strncpy(char* dest, const char* src, size_t dest_size);

#endif // UTILS_H
