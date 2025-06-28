#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void input_parser(char *, char *);
void execute_app(char **);
void greet_user();
void change_dir(char **, char *);
void goto_home_dir();

int main(int argc, char *argv[]) {
    char *cursor = "-> ";
    char current_dir[PATH_MAX];

    greet_user();

    for(;;){
        if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
            perror("Couldn't get current directory");
            return 1;
        }

        printf("[ %s ]%s ", current_dir, cursor);
        char input[MAX_INPUT];
        if (fgets(input, sizeof(input), stdin)) 
            input_parser(input, current_dir);
    }
 
    return 0;
}

void input_parser(char *input, char *current_dir) {
    if (strcmp(input, "exit\n") == 0) {
        printf("Bye bye! :)\n");
        exit(0);
    } else {
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
}

void execute_app(char *args[]) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Couldn't create fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execvp(args[0], args);

        // Child was continuing to run and not stopping after execvp for some reason
        exit(EXIT_SUCCESS);
    } else
        wait(NULL);
}

void change_dir(char *args[], char *current_dir) {
    int success;

    if (args[1][0] == '/') {
        success = chdir(args[1]);
    } else {
        success = chdir(
            strcat(strcat(
                current_dir,
                "/"
            ), 
            args[1]
            )
        );
    }

    if (success < 0) {
        switch (errno) {
            case ENOENT:
                printf("Directory %s does not exist\n", args[1]);
                break;
            case ENOTDIR:
                printf("%s is not a directory\n", args[1]);
                break;
        }
    }
}

void goto_home_dir() {
    chdir(getenv("HOME"));
}

void greet_user() {
    char *username;
    username = getlogin();
    printf("Hi, %s\n\n", username);
}
