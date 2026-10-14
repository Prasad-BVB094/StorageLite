#include "ssd.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

static SSD g_ssd;

void ssd_init(SSD* ssd, uint32_t num_blocks, uint32_t pages_per_block, size_t page_size) {
    if (!ssd) return;
    ssd->num_blocks = num_blocks;
    ssd->pages_per_block = pages_per_block;
    ssd->page_size = page_size;
    
    // Allocate array of blocks
    ssd->blocks = (Block*)safe_malloc(sizeof(Block) * num_blocks);
    
    // Initialize each block
    for (uint32_t i = 0; i < num_blocks; i++) {
        block_init(&ssd->blocks[i], i, pages_per_block, page_size);
    }
}

void ssd_destroy(SSD* ssd) {
    if (!ssd) return;
    if (ssd->blocks) {
        for (uint32_t i = 0; i < ssd->num_blocks; i++) {
            block_destroy(&ssd->blocks[i]);
        }
        free(ssd->blocks);
        ssd->blocks = NULL;
    }
}

void ssd_print_status(const SSD* ssd) {
    if (!ssd) return;
    printf("--- SSD Status ---\n");
    printf("Total Blocks: %u\n", ssd->num_blocks);
    printf("Pages per Block: %u\n", ssd->pages_per_block);
    printf("Page Size: %lu bytes\n", (unsigned long)ssd->page_size);
    printf("Total Capacity: %lu bytes\n", (unsigned long)((size_t)ssd->num_blocks * ssd->pages_per_block * ssd->page_size));
    printf("------------------\n");
}

void ssd_print_fragmentation(const SSD* ssd) {
    if (!ssd) return;
    uint32_t total_pages = ssd->num_blocks * ssd->pages_per_block;
    uint32_t used_pages = 0;
    uint32_t free_pages = 0;
    uint32_t invalid_pages = 0;

    for (uint32_t b = 0; b < ssd->num_blocks; b++) {
        Block* block = &ssd->blocks[b];
        for (uint32_t p = 0; p < block->num_pages; p++) {
            switch (block->pages[p].state) {
                case PAGE_STATE_ALLOCATED: used_pages++; break;
                case PAGE_STATE_FREE:      free_pages++; break;
                case PAGE_STATE_INVALID:   invalid_pages++; break;
            }
        }
    }
    
    double frag_pct = 0.0;
    if (total_pages > 0) {
        frag_pct = ((double)invalid_pages / total_pages) * 100.0;
    }
    
    printf("--- Fragmentation Analysis ---\n");
    printf("Total Pages: %u\n", total_pages);
    printf("Used Pages (Valid): %u\n", used_pages);
    printf("Free Pages: %u\n", free_pages);
    printf("Invalid Pages: %u\n", invalid_pages);
    printf("Fragmentation: %.2f%%\n", frag_pct);
    printf("------------------------------\n");
}

SSD* get_ssd(void) {
    return &g_ssd;
}
