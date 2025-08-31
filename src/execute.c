/* execute.c -- Definition of the execute function provided
 * to main.c and its supporting functions.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

#include "dir.h"
#include "execute.h"
#include "history.h"

/* Takes errno and the program that made execvp not work
 * and prints the right error message for it.
 */
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

/* Checks if the current input given to wordexp_t is a builtin.
 * Returns 1 if it is.
 */
static inline int contains_builtins(wordexp_t *p) {
    return strcmp(p->we_wordv[0], "cd") == 0 ||
           strcmp(p->we_wordv[0], "history") == 0;
}

/* Does a simple execution of a given argv. No pipes no nothing.*/
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
        if (errno == EINTR)
            continue;
        perror("waitpid");
        break;
    }
}

/* Takes care of executing builtins commands.*/
void execute_builtin(wordexp_t *p, char *current_dir, history_t *history) {
    if (strcmp(p->we_wordv[0], "cd") == 0)
        change_dir(p->we_wordv, p->we_wordc, current_dir);
    else if (strcmp(p->we_wordv[0], "history") == 0)
        print_history(history);
}

/* Knows the given input doesn't contain any pipes so it just
 * parses it with wordexp and then does a simple execute of it.
 */
void simple_parse(char *input, char *current_dir, history_t *history) {
    wordexp_t p;

    if (wordexp(input, &p, WRDE_NOCMD) != 0)
        return;

    if (contains_builtins(&p) == 1)
        execute_builtin(&p, current_dir, history);
    else {
        p.we_wordv[p.we_wordc] = NULL; // Terminate with NULL for execvp
        simple_execute(p.we_wordv);
    }

    wordfree(&p);
}

/* Takes a given user input with the amount of pipes it has,
 * splits it into subcommands and parses + executes them accordignly
 * with the piping work around it.
 */
void piped_parse_and_execute(char *input, char *current_dir, int nb_of_pipes,
                             history_t *history) {
    int pipefd[nb_of_pipes][2];
    for (int i = 0; i < nb_of_pipes; i++) {
        if (pipe(pipefd[i]) != 0) {
            perror("pipe");
            return;
        }
    }
    char *token = strtok(input, "|");
    int nb_of_programs = nb_of_pipes + 1;
    pid_t pids[nb_of_programs];

    for (int i = 0; i < nb_of_programs; i++, token = strtok(NULL, "|")) {
        wordexp_t p;
        if (wordexp(token, &p, WRDE_NOCMD) != 0)
            return;
        pids[i] = fork();

        if (pids[i] < 0)
            perror("fork");
        else if (pids[i] == 0) {
            if (i > 0) {
                if (dup2(pipefd[i - 1][0], STDIN_FILENO) == -1)
                    perror("dup2 stdin");
            }

            if (i < nb_of_programs - 1) {
                if (dup2(pipefd[i][1], STDOUT_FILENO) == -1)
                    perror("dup2 stdout");
            }

            for (int j = 0; j < nb_of_programs - 1; j++) {
                close(pipefd[j][0]);
                close(pipefd[j][1]);
            }

            if (contains_builtins(&p) == 1)
                execute_builtin(&p, current_dir, history);
            else {
                p.we_wordv[p.we_wordc] = NULL; // Terminate with NULL for execvp
                execvp(p.we_wordv[0], p.we_wordv);
                execvp_error_catching(errno, p.we_wordv[0]);
            }
        }

        wordfree(&p);
    }

    for (int i = 0; i < nb_of_pipes; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    int status;
    for (int i = 0; i < nb_of_programs; i++)
        while (waitpid(pids[i], &status, 0) == -1) {
        }
}

/* Counts the amount of pipe characters ('|') inside the given user input.*/
int get_nb_of_pipes(char *input) {
    int total_pipes = 0;
    int input_size = strlen(input);
    for (int i = 0; i < input_size; i++) {
        if (input[i] == '|')
            total_pipes++;
    }

    return total_pipes;
}

/* Takes in a user input, checks if contains pipes then gives it
 * to the right function for parsing+execution depending on if
 * it contains pipes or not.
 */
void parse_and_execute(char *input, char *current_dir, history_t *history) {
    int nb_of_pipes = get_nb_of_pipes(input);
    if (nb_of_pipes == 0)
        simple_parse(input, current_dir, history);
    else
        piped_parse_and_execute(input, current_dir, nb_of_pipes, history);
}