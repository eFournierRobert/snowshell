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

FILE *get_hist_file_readptr() {
    char *fname = strcat(getenv("HOME"), "/");
    fname = strcat(fname, ".snowshell_history");
    FILE *fptr;

    if (access(fname, F_OK) != 0) {
        fptr = fopen(fname, "w");
        fclose(fptr);
    }

    return fopen(fname, "r");
}
