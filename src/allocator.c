#include "allocator.h"
#include "utils.h"
#include "wear.h"
#include <stdio.h>
#include <string.h>

static bool find_free_page(SSD* ssd, uint32_t* block_idx, uint32_t* page_idx) {
    uint32_t best_b = wear_get_best_block(ssd);
    if (best_b == UINT32_MAX) return false;
    
    Block* block = &ssd->blocks[best_b];
    for (uint32_t p = 0; p < block->num_pages; p++) {
        if (block->pages[p].state == PAGE_STATE_FREE) {
            *block_idx = best_b;
            *page_idx = p;
            return true;
        }
    }
    return false;
}

static uint32_t count_free_pages(SSD* ssd) {
    uint32_t free_pages = 0;
    for (uint32_t b = 0; b < ssd->num_blocks; b++) {
        Block* block = &ssd->blocks[b];
        for (uint32_t p = 0; p < block->num_pages; p++) {
            if (block->pages[p].state == PAGE_STATE_FREE) {
                free_pages++;
            }
        }
    }
    return free_pages;
}

bool allocate_pages(SSD* ssd, uint32_t num_pages, uint32_t* page_refs, const char* filename) {
    if (!ssd || !page_refs || !filename) return false;
    
    if (count_free_pages(ssd) < num_pages) {
        printf("Error: Insufficient free space. Allocation failed.\n");
        return false;
    }
    
    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t b_idx, p_idx;
        if (!find_free_page(ssd, &b_idx, &p_idx)) {
            // Should not happen due to prior check, but safe fallback
            return false;
        }
        
        Page* page = &ssd->blocks[b_idx].pages[p_idx];
        page->state = PAGE_STATE_ALLOCATED;
        safe_strncpy(page->filename, filename, sizeof(page->filename));
        
        // Encode physical location as (block_idx * pages_per_block) + page_idx
        page_refs[i] = (b_idx * ssd->pages_per_block) + p_idx;
    }
    
    return true;
}

void invalidate_pages(SSD* ssd, uint32_t num_pages, const uint32_t* page_refs) {
    if (!ssd || !page_refs) return;
    
    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t ref = page_refs[i];
        if (ref == UINT32_MAX) continue;
        
        uint32_t b_idx = ref / ssd->pages_per_block;
        uint32_t p_idx = ref % ssd->pages_per_block;
        
        if (b_idx < ssd->num_blocks && p_idx < ssd->blocks[b_idx].num_pages) {
            Page* page = &ssd->blocks[b_idx].pages[p_idx];
            if (page->state == PAGE_STATE_ALLOCATED) {
                page->state = PAGE_STATE_INVALID;
                // Keep data around logically until erased, just like real SSD
            }
        }
    }
}
