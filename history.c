#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "history.h"

FILE *get_hist_file_readptr();

void get_commands_history(struct history *history) {
    char line[MAX_INPUT];
    ssize_t read;
    size_t line_len = MAX_INPUT;
    FILE *fptr = get_hist_file_readptr();

    for (int i = 0; fgets(line, line_len, fptr); i++) {
        line[strlen(line) - 1] = '\0'; // remove newline char
        memcpy(history->hist[i], line, strlen(line));
        history -> length++;
    }
    
    fclose(fptr);
}

void write_hist(struct history *history) {
    char fname[PATH_MAX];
    strcat(fname, getenv("HOME"));
    strcat(fname, "/");
    strcat(fname, ".snowshell_history");
    remove(fname);
    FILE *fptr;
    fptr = fopen(fname, "w");

    for (int i = 0; i < history->length; i++) {
        fprintf(fptr, "%s\n", history->hist[i]);
    }

    fclose(fptr);
}

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


FILE *get_hist_file_readptr() {
    char fname[PATH_MAX];
    strcat(fname, getenv("HOME"));
    strcat(fname, "/");
    strcat(fname, ".snowshell_history");
    FILE *fptr;

    if (access(fname, F_OK) != 0) {
        fptr = fopen(fname, "w");
        fclose(fptr);
    }

    return fopen(fname, "r");
}
