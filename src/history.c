/* history.c -- definition of the history functions provided
 * to main.c and its supporting functions.
 */
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "history.h"

/* Build the absolute path to ~/.snowshell_history file and 
 * stores it in dest.
 */
static inline void build_path_to_hist_file(char *dest) {
    snprintf(dest, PATH_MAX, "%s/.snowshell_history", getenv("HOME"));
}

/* Returns a file pointer to the history file. if the file
 * does not exist, it will be created by this function.
 */
FILE *get_hist_file_readptr() {
    char fname[PATH_MAX] = {0};
    build_path_to_hist_file(fname);

    if (access(fname, F_OK) != 0) {
        FILE *fptr = fopen(fname, "w");
        fclose(fptr);
    }

    return fopen(fname, "r");
}

/* Initialize the given history_t struct with the commands
 * stored inside the history file and how many there is in length.
 */
void get_commands_history(history_t *history) {
    char line[MAX_INPUT] = {0};
    size_t line_len = MAX_INPUT;
    FILE *fptr = get_hist_file_readptr();

    for (int i = 0; fgets(line, line_len, fptr); i++) {
        memcpy(history->hist[i], line, strlen(line) - 1);
        history->length++;
    }

    fclose(fptr);
}

/* Deletes ~/.snowshell_history and remakes it with the command
 * history inside the given history_t struct.
 */
void write_hist(history_t *history) {
    char fname[PATH_MAX] = {0};
    build_path_to_hist_file(fname);
    remove(fname);
    FILE *fptr = fopen(fname, "w");

    for (int i = 0; i < history->length; i++) {
        fprintf(fptr, "%s\n", history->hist[i]);
    }

    fclose(fptr);
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
