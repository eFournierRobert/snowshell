/**
 * @file main.c
 * @author Elliott Fournier-Robert
 * @brief File where the main function of Snowshell is
 */

#include <asm-generic/errno-base.h>
#include <complex.h>
#include <linux/limits.h>
#include <stddef.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

#include "history.h"
#include "inputs.h"
#include "execute.h"

#define MAX_ARGS 128
#define PROMPT_BUFFER 5

/**
 * @brief Builds a shell prompt like: "[ <current_dir> ]<suffix>".
 * 
 * @param[out] dest Output buffer for the prompt .
 * @param[in] destsz Size of @p dest in bytes (including the null terminator).
 * @param[in] current_dir Absolute or display path.
 * @param[in] suffix Prompt suffix (e.g., "->" or "#").
 *
 * @return int Number of chars written (excluding the null terminator), or
 *             -1 if truncation would occur or on invalid arguments.
 *
 * @pre dest != NULL, current_dir != NULL, suffix != NULL, destsz > 0.
 * @note Result format: "[ %s ]%s".
 * @warning Returns -1 if the formatted string does not fit in @p dest.
 */
int build_prompt(char *dest, size_t destsz, char *current_dir, char *suffix) {
    if (!dest || !current_dir || !suffix || destsz == 0)
        return -1;
    
    int n = snprintf(dest, destsz, "[ %s ]%s", current_dir, suffix);
    if (n < 0 || (size_t)n >= destsz)
        return -1;

    return n;
}

/**
 * @brief Prints a small hello message to the user!
 * 
 */
static inline void greet_user() {
    char *username = getlogin();
    printf("Hi, %s\n\n", username);
}

/**
 * @brief Handles the closing of the shell and print a nice bye message.
 * 
 * @param history The history struct that holds the current history to be saved.
 *
 * @note Calls write_hist to save history to ~/.snowshell_history.
 */
static inline void quit(struct history *history) {
    write_hist(history);
    printf("Bye bye! :)\n");
    exit(0);
}


int main() {
    // set up history
    struct history history;
    char *history_content[MAX_INPUT];
    memcpy(history.hist, history_content, sizeof(history_content));
    history.length = 0;

    // set up prompt
    char *prompt_suffix = "-> ";
    int prompt_suffix_size = strlen(prompt_suffix);
    char current_dir[PATH_MAX];

    get_commands_history(&history);
    greet_user();

    for(;;){
        if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
            perror("Couldn't get current directory");
            return 1;
        }

        size_t prompt_size = strlen(current_dir) + prompt_suffix_size + PROMPT_BUFFER;
        char prompt[prompt_size];

        if (build_prompt(prompt, prompt_size, current_dir, prompt_suffix) == -1)
            printf("Couldn't build shell prompt correctly\n");

        printf("%s", prompt);
        char input[MAX_INPUT];
        int ret = snowshell_fgets(input, &history, prompt);
        if (ret == 0) {
            if (strcmp(input, "exit\n") == 0)
                break;
            else if (input[0] != '\n' && input[0] != ' ') {
                input[strlen(input) - 1] = '\0';
                push_to_hist(&history, input);
                parse_and_execute(input, current_dir);
            }
        }
    }

    quit(&history);
}
