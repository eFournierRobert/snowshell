#ifndef HISTORY_H
#include "history.h"
#endif
enum keys {
    UP, DOWN, LEFT, RIGHT, CTRL_C, BACKSPACE, ENTER_KEY
};

int snowshell_fgets(char *input, struct history *history);
