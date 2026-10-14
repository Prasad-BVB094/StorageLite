#include "gc.h"
#include "ssd.h"
#include "file.h"
#include "allocator.h"
#include <stdio.h>
#include <string.h>

void run_garbage_collection(void) {
    SSD* ssd = get_ssd();
    if (!ssd) return;
    
    // Find block with most invalid pages
    uint32_t worst_block_idx = UINT32_MAX;
    uint32_t max_invalid = 0;
    
    for (uint32_t b = 0; b < ssd->num_blocks; b++) {
        Block* block = &ssd->blocks[b];
        uint32_t invalid_count = 0;
        for (uint32_t p = 0; p < block->num_pages; p++) {
            if (block->pages[p].state == PAGE_STATE_INVALID) {
                invalid_count++;
            }
        }
        if (invalid_count > max_invalid) {
            max_invalid = invalid_count;
            worst_block_idx = b;
        }
    }
    
    if (max_invalid == 0 || worst_block_idx == UINT32_MAX) {
        printf("Garbage Collection: No invalid pages to reclaim.\n");
        return;
    }
    
    Block* worst_block = &ssd->blocks[worst_block_idx];
    printf("Garbage Collection: Reclaiming Block %u (Invalid pages: %u)\n", worst_block_idx, max_invalid);
    
    // Relocate valid pages
    for (uint32_t p = 0; p < worst_block->num_pages; p++) {
        Page* page = &worst_block->pages[p];
        if (page->state == PAGE_STATE_ALLOCATED) {
            // Needs relocation
            uint32_t new_ref;
            if (!allocate_pages(ssd, 1, &new_ref, page->filename)) {
                printf("Error: GC Failed to relocate page %u. Not enough free space.\n", p);
                return; // Abort GC
            }
            
            uint32_t new_b = new_ref / ssd->pages_per_block;
            uint32_t new_p = new_ref % ssd->pages_per_block;
            Page* new_page = &ssd->blocks[new_b].pages[new_p];
            
            // Copy data
            memcpy(new_page->data, page->data, ssd->page_size);
            
            // Update File Metadata
            FileEntry* entry = file_find(page->filename);
            if (entry) {
                uint32_t old_ref = (worst_block_idx * ssd->pages_per_block) + p;
                for (uint32_t i = 0; i < entry->page_count; i++) {
                    if (entry->page_refs[i] == old_ref) {
                        entry->page_refs[i] = new_ref;
                        break;
                    }
                }
            }
        }
    }
    
    // Erase the reclaimed block
    block_erase(worst_block);
    printf("Garbage Collection: Block %u erased successfully.\n", worst_block_idx);
}
