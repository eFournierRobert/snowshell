/**
 * @file execute.h
 * @brief Declaration of program execution function provided to main.c
 */

#include "history.h"
#ifndef EXECUTE_H

/**
 * @brief Parses the given input and executes it.
 * 
 * @param input The user input that needs to be executed.
 * @param current_directory The absolute path to the current directory.
 * @param history The current command history. Needed for builtin "history".
 */
void parse_and_execute(char *input, char *current_directory, history_t *history);

#endif