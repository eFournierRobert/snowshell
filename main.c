#include <linux/limits.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void input_parser(char *);
void execute_app(char **);
char *rmv_newline(char *);

int main(int argc, char *argv[]) {
    char *cursor = "-> ";
    bool debug = false;

    if (argc > 1 && strcmp(argv[1], "--debug") == 0)
        debug = true;
    
    for(;;){
        printf("%s ", cursor);
        char input[MAX_INPUT];        
        if (fgets(input, sizeof(input), stdin)) {
            input_parser(input);
        }
    }
 
    return 0;
}

void input_parser(char *input) {
    if (strcmp(input, "exit\n") == 0) {
        printf("Bye bye! :)\n");
        exit(0);
    } else {
        char *token = strtok(input, " ");
        char *args[MAX_INPUT];
        int i = 0;
        input = rmv_newline(input);

        for (; token != NULL; token = strtok(NULL, " ")) 
            args[i++] = token;

        execute_app(args);
        free(input);
        input = NULL;
    }
}

void execute_app(char *args[]) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Couldn't create fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execvp(args[0], args);
    } else
        wait(NULL);
}

char *rmv_newline(char *input) {
    for (int i = 0; input[i] == '\0'; i++) {
        if (input[i] == '\n') 
            input[i] = '\0';
    }

    char *pnewinput = calloc(MAX_INPUT, sizeof(char));
    if (pnewinput == NULL) 
        exit(EXIT_FAILURE);
     
    return pnewinput;
}
