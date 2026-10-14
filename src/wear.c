#include "wear.h"
#include <stdio.h>
#include <stdbool.h>

static bool block_has_free_pages(Block* block) {
    for (uint32_t p = 0; p < block->num_pages; p++) {
        if (block->pages[p].state == PAGE_STATE_FREE) {
            return true;
        }
    }
    return false;
}

uint32_t wear_get_best_block(SSD* ssd) {
    if (!ssd) return UINT32_MAX;
    
    uint32_t best_block_idx = UINT32_MAX;
    uint32_t min_erase_count = UINT32_MAX;
    
    for (uint32_t b = 0; b < ssd->num_blocks; b++) {
        Block* block = &ssd->blocks[b];
        
        if (block_has_free_pages(block)) {
            if (block->erase_count < min_erase_count) {
                min_erase_count = block->erase_count;
                best_block_idx = b;
            }
        }
    }
    
    return best_block_idx;
}

void wear_print_stats(const SSD* ssd) {
    if (!ssd) return;
    
    printf("--- Wear Leveling Stats ---\n");
    for (uint32_t b = 0; b < ssd->num_blocks; b++) {
        printf("Block %u: Erase Count = %u\n", b, ssd->blocks[b].erase_count);
    }
    printf("---------------------------\n");
}
