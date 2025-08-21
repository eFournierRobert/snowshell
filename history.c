#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "history.h"

/**
 * @brief Writes the absolute path to the history file ($HOME/.snowshell_history).
 * 
 * @param[out] dest String where to store the path.
 *
 * @note It assumes the max length of @p dest is PATH_MAX.
 */
static inline void build_path_to_hist_file(char *dest) {
    snprintf(dest, PATH_MAX, "%s/.snowshell_history", getenv("HOME"));
}

/**
 * @brief Get a read FILE pointer to $HOME/.snowshell_history
 * 
 * @return FILE* 
 *
 * @note The FILE pointer being returned is the return of fopen();
 */
FILE *get_hist_file_readptr() {
    char fname[PATH_MAX] = {0};
    build_path_to_hist_file(fname);

    if (access(fname, F_OK) != 0) {
        FILE * fptr = fopen(fname, "w");
        fclose(fptr);
    }

    return fopen(fname, "r");
}

/**
 * @brief Populate the given struct history with the commands
 *        in $HOME/.snowshell_history.
 * 
 * @param[out] history The struct history to populate.
 */
void get_commands_history(struct history *history) {
    char line[MAX_INPUT] = {0};
    ssize_t read = 0;
    size_t line_len = MAX_INPUT;
    FILE *fptr = get_hist_file_readptr();

    for (int i = 0; fgets(line, line_len, fptr); i++) {
        memcpy(history->hist[i], line, strlen(line) - 1);
        history->length++;
    }
    
    fclose(fptr);
}

/**
 * @brief Deletes the history file and replaces it with a new file
 *        that has the updated history.
 * 
 * @param history The struct history with the updated history to store in the file.
 *
 * @note It deletes and rewrite the whole file. It does not just append to it.
 */
void write_hist(struct history *history) {
    char fname[PATH_MAX] = {0};
    build_path_to_hist_file(fname);
    remove(fname);
    FILE *fptr = fopen(fname, "w");

    for (int i = 0; i < history->length; i++) {
        fprintf(fptr, "%s\n", history->hist[i]);
    }

    fclose(fptr);
}

/**
 * @brief Pushes the given command into the history of the given struct history.
 * 
 * @param[out] history The struct history where the commands will be stored.
 * @param[in] input The command to store.
 */
void push_to_hist(struct history *history, char *input) {
    if (history->length < MAX_HIST_SIZE) {
        memcpy(history->hist[history->length], input, strlen(input));
        (history->length)++;
    } else {
        memset(history->hist[0], '\0', strlen(history->hist[0]));
        for (int i = 1; i < history->length; i++) {
            memcpy(history->hist[i - 1], history->hist[i], strlen(history->hist[i]));
            memset(history->hist[i], '\0', strlen(history->hist[i]));
        }

        memcpy(history->hist[history->length], input, strlen(input));
    }
}
