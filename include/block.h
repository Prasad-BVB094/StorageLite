#ifndef BLOCK_H
#define BLOCK_H

#include "page.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t id;
    uint32_t erase_count;
    Page* pages;          // Dynamically allocated array of pages
    uint32_t num_pages;
} Block;

/**
 * Initializes a block and all its pages.
 */
void block_init(Block* block, uint32_t id, uint32_t num_pages, size_t page_size);

/**
 * Frees all pages within a block.
 */
void block_destroy(Block* block);

/**
 * Erases the block (increments wear count, resets all pages).
 */
void block_erase(Block* block);

#endif // BLOCK_H
