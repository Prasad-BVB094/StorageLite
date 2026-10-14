#ifndef WEAR_H
#define WEAR_H

#include "ssd.h"
#include <stdint.h>

/**
 * Returns the index of the block with the lowest erase count that has at least one free page.
 * If no blocks have free pages, returns UINT32_MAX.
 */
uint32_t wear_get_best_block(SSD* ssd);

/**
 * Prints the wear leveling statistics (erase counts for all blocks).
 */
void wear_print_stats(const SSD* ssd);

#endif // WEAR_H
