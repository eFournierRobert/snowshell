#ifndef HISTORY_H
#define HISTORY_H
#include <linux/limits.h>
struct history {
    char hist[500][MAX_INPUT];
    int length;
};

void get_commands_history(struct history *);

void free_history(char **);

#endif
