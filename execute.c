#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

#include "dir.h"
#include "execute.h"

/**
 * @brief Gets the current errno value and checks what is the error
 *        returned by execvp().
 *
 * @param err The current value of errno.
 * @param arg0 The program that was just ran (args[0]).
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

/**
 * @brief Executes the given args without piping.
 *
 * @param args The args to execute.
 */
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

/**
 * @brief Parses the input knowing it doesn't contain any pipes,
 *        then executes it.
 *
 * @param input The current user input in the shell.
 * @param current_dir The current absolute path.
 */
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

/**
 * @brief Executes the given input with piping.
 *        It makes sure to parse it the right way too.
 *
 * @param input The current user input in the shell.
 * @param current_dir The current absolute path.
 * @param nb_of_pipes The number of '|' in the user input.
 */
void piped_parse_and_execute(char *input, char *current_dir, int nb_of_pipes) {
    int pipefd[nb_of_pipes][2];
    for (int i = 0; i < nb_of_pipes; i++) {
        if (pipe(pipefd[i]) != 0) {
            perror("pipe");
            return;
        }
    }
    char *token = strtok(input, "|");
    pid_t pids[nb_of_pipes];
    int nb_of_programs = nb_of_pipes + 1;

    for (int i = 0; i < nb_of_programs; i++, token = strtok(NULL, "|")) {
        pids[i] = fork();
        wordexp_t p;
        if (wordexp(token, &p, WRDE_NOCMD) != 0)
            return;

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

            if (strcmp(p.we_wordv[0], "cd") == 0)
                change_dir(p.we_wordv, p.we_wordc, current_dir);
            else {
                p.we_wordv[p.we_wordc] = NULL; // Terminate with NULL for execvp
                execvp(p.we_wordv[0], p.we_wordv);
                execvp_error_catching(errno, p.we_wordv[0]);
            }
        }

        wordfree(&p);
    }

    for (int i = 0; i < nb_of_programs - 1; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    int status;
    for (int i = 0; i < nb_of_programs; i++)
        while (waitpid(pids[i], &status, 0) == -1) {
        }
}

/**
 * @brief Get the number of pipe character '|' in the given input.
 *
 * @param input The input to check.
 * @return int The number of appearence of '|'.
 */
int get_nb_of_pipes(char *input) {
    int total_pipes = 0;
    int input_size = strlen(input);
    for (int i = 0; i < input_size; i++) {
        if (input[i] == '|')
            total_pipes++;
    }

    return total_pipes;
}

/**
 * @brief Parses the given input then executes it.
 *
 * @param input The user input in the shell.
 * @param current_dir The current absolute path.
 */
void parse_and_execute(char *input, char *current_dir) {
    int nb_of_pipes = get_nb_of_pipes(input);
    if (nb_of_pipes == 0)
        simple_parse(input, current_dir);
    else
        piped_parse_and_execute(input, current_dir, nb_of_pipes);
}