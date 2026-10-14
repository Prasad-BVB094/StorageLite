#include <stdio.h>
#include "cli.h"

#include "ssd.h"
#include "file.h"

int main(void) {
    // Default simulation parameters: 16 blocks, 64 pages per block, 256 bytes per page
    ssd_init(get_ssd(), 16, 64, 256);
    file_manager_init();
    
    // Phase 1: Just start the CLI loop
    cli_start();
    
    // Cleanup on exit
    file_manager_destroy();
    ssd_destroy(get_ssd());
    return 0;
}
