#include "block.h"
#include "utils.h"
#include <stdlib.h>

void block_init(Block* block, uint32_t id, uint32_t num_pages, size_t page_size) {
    if (!block) return;
    block->id = id;
    block->erase_count = 0;
    block->num_pages = num_pages;
    
    // Allocate array of pages
    block->pages = (Page*)safe_malloc(sizeof(Page) * num_pages);
    
    // Initialize each page
    for (uint32_t i = 0; i < num_pages; i++) {
        page_init(&block->pages[i], i, page_size);
    }
}

void block_destroy(Block* block) {
    if (!block) return;
    if (block->pages) {
        for (uint32_t i = 0; i < block->num_pages; i++) {
            page_destroy(&block->pages[i]);
        }
        free(block->pages);
        block->pages = NULL;
    }
}

void block_erase(Block* block) {
    if (!block) return;
    block->erase_count++;
    for (uint32_t i = 0; i < block->num_pages; i++) {
        page_reset(&block->pages[i]);
    }
}
