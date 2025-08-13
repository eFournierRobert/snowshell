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
    printf("\r\033[K");
    fflush(stdout);
    memset(oldinput, '\0', strlen(oldinput));
}

void redraw_line(char *input, int cursor, int len, char *current_dir_cur) {
    printf("\r\033[K");
    printf("%s", current_dir_cur);
    fwrite(input, 1, len, stdout);
    int move_back = len - cursor;
    if (move_back > 0) printf("\033[%dD", move_back);
    fflush(stdout);
}

void go_back_hist(char *input, int *hist_index, struct history *history, int *cursor_pos, char *current_dir_cur) {
    remove_current_input(input);
    (*hist_index)--;
    memcpy(input, history->hist[*hist_index], strlen(history->hist[*hist_index]));
    *cursor_pos = strlen(input);
    printf("%s%s", current_dir_cur, input);
}

void go_forward_hist(char *input, int *hist_index, struct history *history, int *cursor_pos, char *current_dir_cur) {
    if (*hist_index == history->length - 1) {
        remove_current_input(input);
        printf("%s", current_dir_cur);
        memset(input, '\0', MAX_INPUT);
        *cursor_pos = 0;
    } else if (*hist_index < history->length) {
        (*hist_index)++;
        remove_current_input(input);
        memcpy(input, history->hist[*hist_index], strlen(history->hist[*hist_index]));
        *cursor_pos = strlen(input);
        printf("%s%s", current_dir_cur, input);
    }
}

void move_cursor_left(int *cursor_pos) {
    if (*cursor_pos > 0) {
        (*cursor_pos)--;
        printf("\033[D");
    }
}

void move_cursor_right(int *cursor_pos, int current_input_size) {
    if (*cursor_pos < current_input_size) {
        printf("\033[C");
        (*cursor_pos)++;
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
    char seq1, seq2;
    switch (c) {
        case '\x1b':
            seq1 = getchar();
            seq2 = getchar();

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

int snowshell_fgets(char *input, struct history *history, char *current_dir_cur) {
    int quit = -1;
    int hist_index = history->length;
    int cursor = 0;
    int input_length = 0;

    memset(input, '\0', MAX_INPUT);

    while (quit == -1 && input_length < MAX_INPUT) {
        char c = getch();

        switch(c) {
            case ENTER_KEY:
                putchar('\n');
                input[input_length] = '\n';
                quit = 0;
                break;
            case CTRL_C:
                printf("^C\n");
                quit = 1;
                break;
            case BACKSPACE:
                if (input_length > 0) {
                    input[cursor - 1] = '\0';
                    printf("\b \b");
                    cursor--;
                    input_length--;
                }
                break;
            case UP:
                if (hist_index > 0) {
                    go_back_hist(
                        input, 
                        &hist_index, 
                        history, 
                        &cursor,
                        current_dir_cur
                    );
                    input_length = cursor;
                }
                break;
            case DOWN:
                go_forward_hist(
                    input, 
                    &hist_index, 
                    history, 
                    &cursor,
                    current_dir_cur
                );

                input_length = cursor;
                break;
            case LEFT:
                move_cursor_left(&cursor);
                break;
            case RIGHT:
                move_cursor_right(&cursor, input_length);
                break;
            default:
                if (input_length < MAX_INPUT) {
                    if (cursor < input_length) {
                        memmove(&input[cursor+1], &input[cursor], input_length - cursor);
                        input[cursor] = c;
                        cursor++;
                        input_length++;
                        redraw_line(input, cursor, input_length, current_dir_cur);
                    } else {
                        input[input_length++] = c;
                        putchar(c);
                        cursor++;
                    }
                }
                break;
        }
    }

    return quit;
}
