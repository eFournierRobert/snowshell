#ifndef HISTORY_H
#define HISTORY_H
#define MAX_HIST_SIZE 500
#include <linux/limits.h>

struct history {
    char hist[MAX_HIST_SIZE][MAX_INPUT];
    int length;
};

void get_commands_history(struct history *);

void free_history(char **);

void write_hist(struct history *);

void push_to_hist(struct history *, char *);

#endif
