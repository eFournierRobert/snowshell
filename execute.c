#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <wordexp.h>

#include "dir.h"

void execvp_error_catching(int err, char *arg0) {
    switch (err) {
        case ENOENT:
            fprintf(stderr, "snowshell: %s: command not found\n", arg0);
            _exit(127);
            break;
        case EACCES:
            fprintf(stderr, "snowshell: %s: permission denied\n", arg0);
            _exit(126);
            break;
        default:
            fprintf(stderr, "snowshell: %s: %s\n", arg0, strerror(err));
            _exit(126);
            break;
    
    }
}

void simple_execute(char *const args[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    
    if (pid == 0) {
        execvp(args[0], args);
        execvp_error_catching(errno, args[0]);
    }

    int status;
    while (waitpid(pid, &status, 0) == -1) {
        if (errno == EINTR) continue;
        perror("waitpid");
        break;
    }
}

void simple_parse(char *input, char *current_dir) {
    wordexp_t p;
    
    if (wordexp(input, &p, WRDE_NOCMD) != 0)
        return;

    if (strcmp(p.we_wordv[0], "cd") == 0)
        change_dir(p.we_wordv, p.we_wordc, current_dir);
    else {
        p.we_wordv[p.we_wordc] = NULL; // Terminate with NULL for execvp
        simple_execute(p.we_wordv);
    }

    wordfree(&p);
}

void piped_parse_and_execute(char *input, char *current_dir, int nb_of_pipes) {}

int get_nb_of_pipes(char *input) {
    int total_pipes = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '|')
            total_pipes++;
    }

    return total_pipes;
}

void parse_and_execute(char *input, char *current_dir) {
    int nb_of_pipes = get_nb_of_pipes(input);
    if (nb_of_pipes == 0) 
        simple_parse(input, current_dir);
    else
        piped_parse_and_execute(input, current_dir, nb_of_pipes);
}