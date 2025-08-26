#ifndef HISTORY_H
#include "history.h"
#endif
/**
 * @brief Keys that can be pressed during the getch() function.
 *
 * @note The getch() function is inside inputs.c.
 */
enum keys { UP, DOWN, LEFT, RIGHT, CTRL_C, BACKSPACE, ENTER_KEY };

int snowshell_fgets(char *input, struct history *history,
                    char *current_dir_cur);
