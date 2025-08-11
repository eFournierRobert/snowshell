#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "inputs.h"
#include "history.h"

struct termios orig_termios;

void remove_current_input(char *oldinput) {
    for (int i = 0; i < strlen(oldinput); i++)
        printf("\b \b");
}

void go_back_hist(char *input, int *hist_index, struct history *history, int *cursor_pos) {
    remove_current_input(input);
    (*hist_index)--;
    memcpy(input, history->hist[*hist_index], strlen(history->hist[*hist_index]));
    *cursor_pos = strlen(input) - 1;
    printf("%s", input);
}

void go_forward_hist(char *input, int *hist_index, struct history *history, int *cursor_pos) {
    if (*hist_index == history->length - 1) {
        remove_current_input(input);
        *cursor_pos = -1;
        memset(input, '\0', MAX_INPUT);
    } else if (*hist_index < history->length) {
        (*hist_index)++;
        remove_current_input(input);
        memcpy(input, history->hist[*hist_index], strlen(history->hist[*hist_index]));
        *cursor_pos = strlen(input) - 1;
        printf("%s", input);
    }
}

void move_cursor_left(int *cursor_pos) {
    if (*cursor_pos > 0) {
        (*cursor_pos) -= 2;
        printf("\033[D");
    } else {
        *cursor_pos = -1;
    }
}

void move_cursor_right(int *cursor_pos, int current_input_size) {
    if (*cursor_pos < current_input_size) {
        printf("\033[C");
    }
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char getch() {
    enable_raw_mode();
    char c = getchar();
    switch (c) {
        case '\x1b':
            char seq1 = getchar();
            char seq2 = getchar();

            if (seq1 == '[') {
                switch (seq2) {
                    case 'A': 
                        c = UP; 
                        break;
                    case 'B': 
                        c = DOWN; 
                        break;
                    case 'C': 
                        c = RIGHT; 
                        break;
                    case 'D': 
                        c = LEFT; 
                        break;
                }
            }
            break;
        case 13:
            c = ENTER_KEY;
            break;
        case 127:
            c = BACKSPACE;
            break;
        case 3:
            c = CTRL_C;
            break;
    }

    disable_raw_mode();
    return c;
}

int snowshell_fgets(char *input, struct history *history) {
    int quit = -1;
    int hist_index = history->length;

    memset(input, '\0', MAX_INPUT);

    for (int i = 0; i < MAX_INPUT && quit == -1; i++) {
        char c = getch();

        switch(c) {
            case ENTER_KEY:
                printf("\n");
                input[i] = '\n';
                quit = 0;
                break;
            case CTRL_C:
                printf("^C\n");
                quit = 1;
                break;
            case BACKSPACE:
                if (i > 0) {
                   input[i - 1] = '\0';
                    printf("\b \b");
                    i -= 2;
                } else {
                    i--;
                }
                break;
            case UP:
                if (hist_index > 0) {
                    go_back_hist(
                        input, 
                        &hist_index, 
                        history, 
                        &i
                    );
                }
                break;
            case DOWN:
                go_forward_hist(
                    input, 
                    &hist_index, 
                    history, 
                    &i
                );
                break;
            case LEFT:
                move_cursor_left(&i);
                break;
            case RIGHT:
                move_cursor_right(&i, strlen(input));
                break;
            default:
                input[i] = c;
                printf("%c", c);
                break;
        }
    }

    return quit;
}
