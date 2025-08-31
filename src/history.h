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
 *        commands history from the file .snowshell_history and the length of
 *        it.
 *
 * @param history The history_t where to store the data.
 * @param hist_file_path Absolute file path to history file. Can be set to 
 *                       NULL to get default $HOME/.snowshell_history.
 */
void get_commands_history(history_t *history, char *hist_file_path);

/**
 * @brief Writes the content of the history struct to the history file.
 *
 * @param history The history_t struct that needs to be stored.
 * @param hist_file_path Absolute file path to history file. Can be set to 
 *                       NULL to get default $HOME/.snowshell_history.
 *
 * @warning This functions deletes ~/.snowshell_history and rewrites in
 * completely.
 */
void write_hist(history_t *history, char *hist_file_path);

/**
 * @brief Push the given input to the given history struct.
 *
 * @param history The history_t struct where it needs to be stored.
 * @param input The input to store on it.
 */
void push_to_hist(history_t *history, char *input);

/**
 * @brief Prints the given command history along with its index to stdout.
 *
 * @param history The current command history.
 */
void print_history(history_t *history);

#ifdef TESTING
    FILE *get_hist_file_readptr(char *);
#endif

#endif
