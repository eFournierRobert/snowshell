#include <asm-generic/errno-base.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"
#include "inputs.h"
#include "dir.h"

void input_parser(char *, char *);
void execute_app(char **);
void greet_user();
void quit();
void build_cursor(char *, char *, char *);

int main() {
    struct history history;
    char *history_content[MAX_INPUT];
    memcpy(history.hist, history_content, sizeof(history_content));
    history.length = 0;

    char *cursor = "-> ";
    char current_dir[PATH_MAX];

    get_commands_history(&history);
    greet_user();

    for(;;){
        if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
            perror("Couldn't get current directory");
            return 1;
        }

        char current_dir_cur[strlen(current_dir) + strlen(cursor)];
        build_cursor(current_dir_cur, current_dir, cursor);

        printf("%s", current_dir_cur);
        char input[MAX_INPUT];
        int ret = snowshell_fgets(input, &history, current_dir_cur);
        if (ret == 0) {
            if (strcmp(input, "exit\n") == 0)
                break;
            else if (input[0] != '\n')
                input_parser(input, current_dir);
        } else if (ret == 1)
            continue;
    }

    quit();
}

void build_cursor(char *dest, char *current_dir, char *cursor) {
    dest[0] = '\0';
    strcat(dest, "[ ");
    strcat(dest, current_dir);
    strcat(dest, " ]");
    strcat(dest, cursor);
}

void input_parser(char *input, char *current_dir) {
    char *token = strtok(input, " ");
    char *tempargs[MAX_INPUT];
    int i = 0;

    for (; token != NULL; token = strtok(NULL, " ")) { 
        if (token[strlen(token) - 1] == '\n')
            token[strlen(token) - 1] = '\0';
        tempargs[i++] = token;
    }

    if (strcmp(tempargs[0], "cd") == 0) {
        if (i < 2 || tempargs[1][0] == '~')
            goto_home_dir();
        else
            change_dir(tempargs, current_dir);
    } else {
        char *args[i+1];
        for (int j = 0; j < i; j++) 
            args[j] = tempargs[j];
        args[i] = NULL; // NULL terminating for execvp

        execute_app(args);
        input = NULL;
    }
}

void execute_app(char *args[]) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Couldn't create fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (execvp(args[0], args) == -1 && errno == ENOENT) {
            printf("Snowshell: command not found: %s\n", args[0]);
        }

        // Child was continuing to run and not stopping after execvp for some reason
        exit(EXIT_SUCCESS);
    } else
        wait(NULL);
}

void greet_user() {
    char *username = getlogin();
    printf("Hi, %s\n\n", username);
}

void quit() {
    printf("Bye bye! :)\n");
    exit(0);
}
