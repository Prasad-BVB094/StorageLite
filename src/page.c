#include "page.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>

void page_init(Page* page, uint32_t id, size_t page_size) {
    if (!page) return;
    page->id = id;
    page->state = PAGE_STATE_FREE;
    memset(page->filename, 0, sizeof(page->filename));
    
    // Allocate simulated data buffer
    page->data = (char*)safe_malloc(page_size);
    memset(page->data, 0, page_size);
}

void page_destroy(Page* page) {
    if (!page) return;
    if (page->data) {
        free(page->data);
        page->data = NULL;
    }
}

void page_reset(Page* page) {
    if (!page) return;
    page->state = PAGE_STATE_FREE;
    memset(page->filename, 0, sizeof(page->filename));
    // We do not free the data buffer here, just clear it logically or leave it be.
    // In a real SSD, an erase clears the page.
    if (page->data) {
        page->data[0] = '\0'; // Logical clear
    }
}
