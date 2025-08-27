/* inputs.c -- Definition of the inputs function provided to
 * main.c and its supporting functions.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "history.h"
#include "inputs.h"

/* Enum of keys that can be returned by getch() if not a char.*/
enum keys { UP, DOWN, LEFT, RIGHT, CTRL_C, BACKSPACE, ENTER_KEY };

/* Needed to mess around with terminal raw mode.*/
struct termios orig_termios;

/* Removes the current input from stdout. Assumes that oldinput is
 * what is currently on the screen.
 */
void remove_current_input(char *oldinput) {
    printf("\r\033[K");
    fflush(stdout);
    memset(oldinput, '\0', strlen(oldinput));
}

/* Redraws the given input to the screen while placing the cursor
 * at the right position on the screen. Needs the prompt because it
 * completely rewrite the line on the screen.
 */
void redraw_line(char *input, int cursor, int len, char *prompt) {
    printf("\r\033[K");
    printf("%s", prompt);
    fwrite(input, 1, len, stdout);
    int move_back = len - cursor;
    if (move_back > 0)
        printf("\033[%dD", move_back);
    fflush(stdout);
}

/* Takes a given history and the current index inside it, then rewrites
 * input and what is on the screen with the command stored at hist_index - 1.
 * Updates hist_index along the way.
 */
void go_back_hist(char *input, int *hist_index, history_t *history,
                  int *cursor_pos, char *prompt) {
    if (*hist_index > 0) {
        remove_current_input(input);
        (*hist_index)--;
        memcpy(input, history->hist[*hist_index],
               strlen(history->hist[*hist_index]));
        *cursor_pos = strlen(input);
        printf("%s%s", prompt, input);
    }
}

/* Takes a given history and the current index inside it, then rewrites
 * input and what is on the screen with the command stored at hist_index + 1.
 * Updates hist_index along the way.
 */
void go_forward_hist(char *input, int *hist_index, history_t *history,
                     int *cursor_pos, char *prompt) {
    if (*hist_index == history->length - 1) {
        remove_current_input(input);
        printf("%s", prompt);
        memset(input, '\0', MAX_INPUT);
        *cursor_pos = 0;
    } else if (*hist_index < history->length) {
        (*hist_index)++;
        remove_current_input(input);
        memcpy(input, history->hist[*hist_index],
               strlen(history->hist[*hist_index]));
        *cursor_pos = strlen(input);
        printf("%s%s", prompt, input);
    }
}

/* Moves the cursor left and updates the given cursor_pos.
 * Will check cursor_pos as to not go to far left.
 */
void move_cursor_left(int *cursor_pos) {
    if (*cursor_pos > 0) {
        (*cursor_pos)--;
        printf("\033[D");
    }
}
/* Moves the cursor right and updates the given cursor_pos. Takes
 * the current input size as to not go to far to the right.
 */
void move_cursor_right(int *cursor_pos, int current_input_size) {
    if (*cursor_pos < current_input_size) {
        printf("\033[C");
        (*cursor_pos)++;
    }
}

/* Disables termios raw mode.*/
void disable_raw_mode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

/* Enable termios raw mode.*/
void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* My own version of what I saw Windows getch() was. Will return
 * either a char or a value from the keys enum.
 */
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

/* The snowshell fgets itself. Will loop inside until it quits (CTRL_C or ENTER)
 * or the current input length becomes equals to MAX_INPUT.
 */
int snowshell_fgets(char *input, history_t *history, char *prompt) {
    int quit = -1;
    int hist_index = history->length;
    int cursor = 0;
    int input_length = 0;

    memset(input, '\0', MAX_INPUT);

    while (quit == -1 && input_length < MAX_INPUT) {
        char c = getch();

        switch (c) {
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
            go_back_hist(input, &hist_index, history, &cursor, prompt);
            input_length = cursor;
            break;
        case DOWN:
            go_forward_hist(input, &hist_index, history, &cursor, prompt);

            input_length = cursor;
            break;
        case LEFT:
            move_cursor_left(&cursor);
            break;
        case RIGHT:
            move_cursor_right(&cursor, input_length);
            break;
        default: // getch returned a char.
            if (input_length < MAX_INPUT) {
                if (cursor < input_length) {
                    memmove(&input[cursor + 1], &input[cursor],
                            input_length - cursor);
                    input[cursor] = c;
                    cursor++;
                    input_length++;
                    redraw_line(input, cursor, input_length, prompt);
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
