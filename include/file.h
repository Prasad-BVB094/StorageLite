#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_FILENAME_LEN 64

// Struct for metadata about a file
typedef struct FileEntry {
    char filename[MAX_FILENAME_LEN];
    uint32_t size;           // Logical size in bytes
    uint32_t* page_refs;     // Dynamically allocated array of physical page IDs
    uint32_t page_count;     // Number of pages allocated
    struct FileEntry* next;  // Linked list pointer
} FileEntry;

// Struct to manage the collection of files
typedef struct {
    FileEntry* head;
    uint32_t total_files;
} FileManager;

/**
 * Initializes the global file manager.
 */
void file_manager_init(void);

/**
 * Destroys the global file manager, freeing all file metadata.
 */
void file_manager_destroy(void);

/**
 * Gets the global file manager instance.
 */
FileManager* get_file_manager(void);

/**
 * Creates a new file metadata entry.
 * Returns true if successful, false if duplicate or error.
 */
bool file_create(const char* filename, uint32_t size);

/**
 * Finds a file by filename.
 * Returns pointer to FileEntry or NULL if not found.
 */
FileEntry* file_find(const char* filename);

/**
 * Deletes a file's metadata and marks its pages as invalid.
 * Returns true on success, false if not found.
 */
bool file_delete(const char* filename);

/**
 * Lists all active files.
 */
void file_list(void);

/**
 * Prints metadata for a specific file.
 */
void file_search(const char* filename);

/**
 * Writes a string of data to the file, mapping it across allocated pages.
 */
bool file_write(const char* filename, const char* data);

/**
 * Reads the string of data from the file's allocated pages and prints it.
 */
void file_read(const char* filename);

#endif // FILE_H
