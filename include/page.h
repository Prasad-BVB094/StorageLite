#ifndef PAGE_H
#define PAGE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    PAGE_STATE_FREE,
    PAGE_STATE_ALLOCATED,
    PAGE_STATE_INVALID
} PageState;

typedef struct {
    uint32_t id;
    PageState state;
    char filename[64];
    char* data;        // Dynamically allocated buffer for simulated data
} Page;

/**
 * Initializes a page struct.
 */
void page_init(Page* page, uint32_t id, size_t page_size);

/**
 * Frees the dynamically allocated contents of a page.
 */
void page_destroy(Page* page);

/**
 * Resets a page to the free state (used during garbage collection/erase).
 */
void page_reset(Page* page);

#endif // PAGE_H
