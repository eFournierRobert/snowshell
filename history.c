#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "history.h"

FILE *get_hist_file_readptr();

void get_commands_history(char **hist) {
    char line[26];
    ssize_t read;
    size_t line_len = 26;
    FILE *fptr = get_hist_file_readptr();

    for (int i = 0; fgets(line, line_len, fptr); i++)
        hist[i] = line;
    
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
