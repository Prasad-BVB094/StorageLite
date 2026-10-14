#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ssd.h"
#include "file.h"
#include "wear.h"
#include "gc.h"

#define MAX_INPUT 256
#define MAX_ARGS 10

static void print_help(void) {
    printf("StorageLite SSD Simulator Commands:\n");
    printf("  create <filename> <size> - Create a file\n");
    printf("  write <filename> <data>  - Write data to file\n");
    printf("  read <filename>          - Read file data\n");
    printf("  delete <filename>        - Delete a file\n");
    printf("  search <filename>        - Display file metadata\n");
    printf("  list                     - List all files\n");
    printf("  status                   - Display SSD status\n");
    printf("  fragmentation            - Display fragmentation stats\n");
    printf("  gc                       - Run garbage collection\n");
    printf("  wear                     - Display wear statistics\n");
    printf("  help                     - Show this help message\n");
    printf("  exit                     - Exit simulator\n");
}

void cli_start(void) {
    char input[MAX_INPUT];
    char* args[MAX_ARGS];
    int arg_count;

    printf("Welcome to StorageLite SSD Simulator. Type 'help' for commands.\n");

    while (true) {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Strip newline
        input[strcspn(input, "\n")] = 0;

        // Tokenize input
        arg_count = 0;
        char* token = strtok(input, " ");
        while (token != NULL && arg_count < MAX_ARGS) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }

        if (arg_count == 0) {
            continue;
        }

        // Process commands
        if (strcmp(args[0], "exit") == 0) {
            break;
        } else if (strcmp(args[0], "help") == 0) {
            print_help();
        } else if (strcmp(args[0], "status") == 0) {
            ssd_print_status(get_ssd());
        } else if (strcmp(args[0], "create") == 0) {
            if (arg_count < 3) {
                printf("Usage: create <filename> <size>\n");
            } else {
                char* endptr;
                long parsed_size = strtol(args[2], &endptr, 10);
                if (*endptr != '\0' || parsed_size < 0) {
                    printf("Error: Size must be a positive integer.\n");
                } else {
                    file_create(args[1], (uint32_t)parsed_size);
                }
            }
        } else if (strcmp(args[0], "write") == 0) {
            if (arg_count < 3) {
                printf("Usage: write <filename> <data>\n");
            } else {
                // Reconstruct data string since it might have spaces and was tokenized
                char data_buf[MAX_INPUT] = {0};
                for (int i = 2; i < arg_count; i++) {
                    strcat(data_buf, args[i]);
                    if (i < arg_count - 1) strcat(data_buf, " ");
                }
                file_write(args[1], data_buf);
            }
        } else if (strcmp(args[0], "read") == 0) {
            if (arg_count < 2) {
                printf("Usage: read <filename>\n");
            } else {
                file_read(args[1]);
            }
        } else if (strcmp(args[0], "delete") == 0) {
            if (arg_count < 2) {
                printf("Usage: delete <filename>\n");
            } else {
                file_delete(args[1]);
            }
        } else if (strcmp(args[0], "search") == 0) {
            if (arg_count < 2) {
                printf("Usage: search <filename>\n");
            } else {
                file_search(args[1]);
            }
        } else if (strcmp(args[0], "list") == 0) {
            file_list();
        } else if (strcmp(args[0], "fragmentation") == 0) {
            ssd_print_fragmentation(get_ssd());
        } else if (strcmp(args[0], "wear") == 0) {
            wear_print_stats(get_ssd());
        } else if (strcmp(args[0], "gc") == 0) {
            run_garbage_collection();
        } else {
            printf("Error: Unknown command or not yet implemented.\n");
        }
    }
}
