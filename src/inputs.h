#ifndef INPUTS_H
#define INPUTS_H

#ifndef HISTORY_H
#include "history.h"
#endif

/**
 * @brief Keys that can be pressed during the getch() function.
 *
 * @note The getch() function is inside inputs.c.
 */
enum keys { UP, DOWN, LEFT, RIGHT, CTRL_C, BACKSPACE, ENTER_KEY };

/**
 * @brief The fgets function used by the shell instead of the default fgets().
 *        It detects things like arrow keys or key combination and deals with them automatically.
 * 
 * @param input The string where the user input should be stored.
 * @param history The current history struct the current up to date history.
 * @param current_dir_cur The shell prompt.
 * @return int Returns 0 if the enter key was pressed, 1 if CTRL_C was pressed.
 *
 * @warning If it returns 0, it doesn't mean that input will have anything in it. 
 *          It only means that the enter key was pressed. Empty input should be dealt with.
 */
int snowshell_fgets(char *input, history_t *history,
                    char *prompt);

#endif