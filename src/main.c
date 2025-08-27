/* main.c -- Snowshell main loop
 *  
 *  Where the main loop of the shell and some high level functions reside.
 */

#include <asm-generic/errno-base.h>
#include <complex.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

#include "execute.h"
#include "history.h"
#include "inputs.h"

#define MAX_ARGS 128
#define PROMPT_BUFFER 5

/* Builds the shell prompt correctly then stores it inside dest.*/
int build_prompt(char *dest, size_t destsz, char *current_dir, char *suffix) {
    if (!dest || !current_dir || !suffix || destsz == 0)
        return -1;

    int n = snprintf(dest, destsz, "[ %s ]%s", current_dir, suffix);
    if (n < 0 || (size_t)n >= destsz)
        return -1;

    return n;
}

/* Greets the user when opening the terminal because why not? */
static inline void greet_user() {
    char *username = getlogin();
    printf("Hi, %s\n\n", username);
}
/* Exit cleanly the shell. Writes the history to the file then exit. */
static inline void quit(history_t *history) {
    write_hist(history);
    printf("Bye bye! :)\n");
    exit(0);
}

/* Parses the command line argument given to the shell.*/
void cli_args_parser(int argc, char **argv) {
    if (argc > 0) {
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "--greet") == 0)
                greet_user();
        }
    }
}

int main(int argc, char *argv[]) {
    cli_args_parser(argc, argv);

    // set up history
    history_t history = {0};
    char *history_content[MAX_INPUT] = {0};
    memcpy(history.hist, history_content, sizeof(history_content));
    history.length = 0;

    // set up prompt
    char *prompt_suffix = "-> ";
    int prompt_suffix_size = strlen(prompt_suffix);
    char current_dir[PATH_MAX] = {0};

    get_commands_history(&history);

    for (;;) {
        if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
            perror("Couldn't get current directory");
            return 1;
        }

        size_t prompt_size =
            strlen(current_dir) + prompt_suffix_size + PROMPT_BUFFER;
        char prompt[prompt_size];
        memset(prompt, '\0', prompt_size);

        if (build_prompt(prompt, prompt_size, current_dir, prompt_suffix) == -1)
            printf("Couldn't build shell prompt correctly\n");

        printf("%s", prompt);
        char input[MAX_INPUT] = {0};
        int ret = snowshell_fgets(input, &history, prompt);
        if (ret == 0) {
            if (strcmp(input, "exit\n") == 0)
                break;
            else if (input[0] != '\n' && input[0] != ' ') {
                input[strlen(input) - 1] = '\0';
                push_to_hist(&history, input);
                parse_and_execute(input, current_dir, &history);
            }
        }
    }

    quit(&history);
}
