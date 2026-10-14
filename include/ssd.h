#ifndef SSD_H
#define SSD_H

#include "block.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    Block* blocks;          // Dynamically allocated array of blocks
    uint32_t num_blocks;
    uint32_t pages_per_block;
    size_t page_size;
} SSD;

/**
 * Initializes the global SSD structure.
 */
void ssd_init(SSD* ssd, uint32_t num_blocks, uint32_t pages_per_block, size_t page_size);

/**
 * Destroys the SSD, freeing all blocks and pages.
 */
void ssd_destroy(SSD* ssd);

/**
 * Prints the status of the SSD.
 */
void ssd_print_status(const SSD* ssd);

/**
 * Prints fragmentation statistics.
 */
void ssd_print_fragmentation(const SSD* ssd);

/**
 * Gets a global pointer to the SSD instance.
 */
SSD* get_ssd(void);

#endif // SSD_H
