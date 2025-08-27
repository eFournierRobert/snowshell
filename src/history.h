/**
 * @file history.h
 * @brief Declarations of history functions and definition of the history 
 *        struct provided to main.c.
 */

#ifndef HISTORY_H
#define HISTORY_H
#include <limits.h>
#define MAX_HIST_SIZE 500

/**
 * @brief Struct containing the commands history.
 * - hist[MAX_HIST_SIZE][MAX_INPUT] is the string array of commands in history.
 * - length is the length of hist.
 */
typedef struct history {
    char hist[MAX_HIST_SIZE][MAX_INPUT];
    int length;
} history_t;

/**
 * @brief Takes in a pointer to a history_t struct and stores inside the 
 *        commands history from the file .snowshell_history and the length of it.
 * 
 * @param history The history_t where to store the data.
 */
void get_commands_history(history_t *history);

/**
 * @brief Writes the content of the history struct to ~/.snowshell_history.
 * 
 * @param history The history_t struct that needs to be stored.
 *
 * @warning This functions deletes ~/.snowshell_history and rewrites in completely.
 */
void write_hist(history_t *history);

/**
 * @brief Push the given input to the given history struct.
 * 
 * @param history The history_t struct where it needs to be stored.
 * @param input The input to store on it.
 */
void push_to_hist(history_t *history, char *input);

void print_history(history_t *history);

#endif
