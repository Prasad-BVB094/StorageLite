#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "ssd.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * Allocates 'num_pages' from the SSD.
 * The allocated physical page IDs are stored in the 'page_refs' array.
 * Returns true if successful, false if insufficient space.
 */
bool allocate_pages(SSD* ssd, uint32_t num_pages, uint32_t* page_refs, const char* filename);

/**
 * Invalidates the pages specified in the 'page_refs' array.
 */
void invalidate_pages(SSD* ssd, uint32_t num_pages, const uint32_t* page_refs);

#endif // ALLOCATOR_H
