#include "file.h"
#include "utils.h"
#include "ssd.h"
#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FileManager g_file_manager;

void file_manager_init(void) {
    g_file_manager.head = NULL;
    g_file_manager.total_files = 0;
}

void file_manager_destroy(void) {
    FileEntry* current = g_file_manager.head;
    while (current != NULL) {
        FileEntry* next = current->next;
        if (current->page_refs) {
            free(current->page_refs);
        }
        free(current);
        current = next;
    }
    g_file_manager.head = NULL;
    g_file_manager.total_files = 0;
}

FileManager* get_file_manager(void) {
    return &g_file_manager;
}

FileEntry* file_find(const char* filename) {
    if (!filename) return NULL;
    FileEntry* current = g_file_manager.head;
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

bool file_create(const char* filename, uint32_t size) {
    if (!filename || strlen(filename) >= MAX_FILENAME_LEN) {
        printf("Error: Invalid filename length.\n");
        return false;
    }
    
    if (file_find(filename) != NULL) {
        printf("Error: File '%s' already exists.\n", filename);
        return false;
    }
    
    // In Phase 3, we just create metadata. (Allocation happens in Phase 4).
    // But we need to know how many pages it would take.
    SSD* ssd = get_ssd();
    uint32_t pages_needed = (size + ssd->page_size - 1) / ssd->page_size;
    if (size == 0) pages_needed = 0;
    
    FileEntry* new_file = (FileEntry*)safe_malloc(sizeof(FileEntry));
    safe_strncpy(new_file->filename, filename, MAX_FILENAME_LEN);
    new_file->size = size;
    new_file->page_count = pages_needed;
    
    if (pages_needed > 0) {
        new_file->page_refs = (uint32_t*)safe_malloc(sizeof(uint32_t) * pages_needed);
        // Initialize to sentinel
        for (uint32_t i = 0; i < pages_needed; i++) {
            new_file->page_refs[i] = UINT32_MAX;
        }
        
        if (!allocate_pages(ssd, pages_needed, new_file->page_refs, filename)) {
            free(new_file->page_refs);
            free(new_file);
            return false;
        }
    } else {
        new_file->page_refs = NULL;
    }
    
    // Add to front of list
    new_file->next = g_file_manager.head;
    g_file_manager.head = new_file;
    g_file_manager.total_files++;
    
    printf("Created file '%s' (Size: %u bytes, Pages: %u).\n", filename, size, pages_needed);
    return true;
}

bool file_delete(const char* filename) {
    if (!filename) return false;
    
    FileEntry* current = g_file_manager.head;
    FileEntry* prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            // Found it. 
            if (current->page_refs) {
                invalidate_pages(get_ssd(), current->page_count, current->page_refs);
            }
            
            // Remove from list
            if (prev == NULL) {
                g_file_manager.head = current->next;
            } else {
                prev->next = current->next;
            }
            
            if (current->page_refs) {
                free(current->page_refs);
            }
            free(current);
            g_file_manager.total_files--;
            
            printf("Deleted file '%s'.\n", filename);
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    printf("Error: File '%s' not found.\n", filename);
    return false;
}

void file_list(void) {
    printf("--- File List (%u files) ---\n", g_file_manager.total_files);
    FileEntry* current = g_file_manager.head;
    while (current != NULL) {
        printf("- %s (Size: %u bytes)\n", current->filename, current->size);
        current = current->next;
    }
    printf("---------------------------\n");
}

void file_search(const char* filename) {
    FileEntry* entry = file_find(filename);
    if (!entry) {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }
    printf("--- File Metadata ---\n");
    printf("Filename: %s\n", entry->filename);
    printf("Size: %u bytes\n", entry->size);
    printf("Pages Allocated: %u\n", entry->page_count);
    printf("---------------------\n");
}

bool file_write(const char* filename, const char* data) {
    FileEntry* entry = file_find(filename);
    if (!entry) {
        printf("Error: File '%s' not found.\n", filename);
        return false;
    }
    
    if (!data) return false;
    
    size_t data_len = strlen(data);
    if (data_len > entry->size) {
        printf("Error: Data size (%lu) exceeds file size (%u).\n", (unsigned long)data_len, entry->size);
        return false;
    }
    
    SSD* ssd = get_ssd();
    size_t written = 0;
    
    for (uint32_t i = 0; i < entry->page_count && written < data_len; i++) {
        uint32_t old_ref = entry->page_refs[i];
        
        // Out-of-place update: Invalidate the old page if it was already allocated
        if (old_ref != UINT32_MAX) {
            invalidate_pages(ssd, 1, &old_ref);
        }
        
        // Allocate a new page
        uint32_t new_ref;
        if (!allocate_pages(ssd, 1, &new_ref, filename)) {
            printf("Error: Insufficient free space for out-of-place update.\n");
            return false;
        }
        
        entry->page_refs[i] = new_ref;
        
        uint32_t b_idx = new_ref / ssd->pages_per_block;
        uint32_t p_idx = new_ref % ssd->pages_per_block;
        
        Page* page = &ssd->blocks[b_idx].pages[p_idx];
        
        size_t to_write = data_len - written;
        if (to_write > ssd->page_size) {
            to_write = ssd->page_size;
        }
        
        memcpy(page->data, data + written, to_write);
        
        // Ensure null termination if there's space (though memory is zeroed on init)
        if (to_write < ssd->page_size) {
            page->data[to_write] = '\0';
        }
        
        written += to_write;
    }
    
    printf("Wrote %lu bytes to '%s'.\n", (unsigned long)written, filename);
    return true;
}

void file_read(const char* filename) {
    FileEntry* entry = file_find(filename);
    if (!entry) {
        printf("Error: File '%s' not found.\n", filename);
        return;
    }
    
    SSD* ssd = get_ssd();
    printf("Data for '%s': ", filename);
    
    for (uint32_t i = 0; i < entry->page_count; i++) {
        uint32_t ref = entry->page_refs[i];
        if (ref == UINT32_MAX) continue;
        
        uint32_t b_idx = ref / ssd->pages_per_block;
        uint32_t p_idx = ref % ssd->pages_per_block;
        
        Page* page = &ssd->blocks[b_idx].pages[p_idx];
        
        // Print character by character up to page size, stopping at null terminator
        for (size_t c = 0; c < ssd->page_size; c++) {
            if (page->data[c] == '\0') break;
            putchar(page->data[c]);
        }
    }
    printf("\n");
}
