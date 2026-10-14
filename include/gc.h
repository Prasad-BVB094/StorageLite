#ifndef GC_H
#define GC_H

/**
 * Runs the garbage collection algorithm.
 * Identifies the most fragmented block, relocates valid pages, and erases it.
 */
void run_garbage_collection(void);

#endif // GC_H
