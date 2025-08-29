/* history.c -- definition of the history functions provided
 * to main.c and its supporting functions.
 */
#include <limits.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "history.h"

/* Returns a file pointer to the history file. if the file
 * does not exist, it will be created by this function.
 */
FILE *get_hist_file_readptr(char *hist_file_path) {
    if (access(hist_file_path, F_OK) != 0) {
        FILE *fptr = fopen(hist_file_path, "w");
        fclose(fptr);
    }

    return fopen(hist_file_path, "r");
}

/* Initialize the given history_t struct with the commands
 * stored inside the history file and how many there is in length.
 */
void get_commands_history(history_t *history, char *hist_file_path) {
    int file_path_on_heap = 0;
    if (hist_file_path == NULL) {
        hist_file_path = calloc(sizeof(char), PATH_MAX);
        
        if (hist_file_path == NULL) { exit(1); }

        file_path_on_heap = 1;
        snprintf(hist_file_path, PATH_MAX, "%s/.snowshell_history", getenv("HOME"));
    }

    char line[MAX_INPUT] = {0};
    size_t line_len = MAX_INPUT;
    FILE *fptr = get_hist_file_readptr(hist_file_path);

    for (int i = 0; fgets(line, line_len, fptr); i++) {
        memcpy(history->hist[i], line, strlen(line) - 1);
        history->length++;
    }

    fclose(fptr);
    if (file_path_on_heap == 1)
        free(hist_file_path);
}

/* Deletes ~/.snowshell_history and remakes it with the command
 * history inside the given history_t struct.
 */
void write_hist(history_t *history, char *hist_file_path) {
    int file_path_on_heap = 0;
    if (hist_file_path == NULL) {
        hist_file_path = calloc(sizeof(char), PATH_MAX);
        
        if (hist_file_path == NULL) { exit(1); }

        file_path_on_heap = 1;
        snprintf(hist_file_path, PATH_MAX, "%s/.snowshell_history", getenv("HOME"));
    }

    remove(hist_file_path);
    FILE *fptr = fopen(hist_file_path, "w");

    for (int i = 0; i < history->length; i++) {
        fprintf(fptr, "%s\n", history->hist[i]);
    }

    fclose(fptr);
    if (file_path_on_heap == 1)
        free(hist_file_path);
}

/* Pushes the given user input at the end of the given history_t struct
 * history. If the history isn't full, it will increment length.
 */
void push_to_hist(history_t *history, char *input) {
    if (history->length < MAX_HIST_SIZE) {
        memcpy(history->hist[history->length], input, strlen(input));
        (history->length)++;
    } else {
        memset(history->hist[0], '\0', strlen(history->hist[0]));
        for (int i = 1; i < history->length; i++) {
            memcpy(history->hist[i - 1], history->hist[i],
                   strlen(history->hist[i]));
            memset(history->hist[i], '\0', strlen(history->hist[i]));
        }

        memcpy(history->hist[history->length], input, strlen(input));
    }
}

/* Prints the history on stdout with its index.*/
void print_history(history_t *history) {
    for (int i = 0; i < history->length; i++)
        printf("%d %s\n", i + 1, history->hist[i]);
}
