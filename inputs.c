#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "inputs.h"
#include "history.h"

struct termios orig_termios;

/**
 * @brief Reset the current input and removes it from stdout.
 * 
 * @param oldinput The input to reset. Memsets it to '\0'.
 */
void remove_current_input(char *oldinput) {
    printf("\r\033[K");
    fflush(stdout);
    memset(oldinput, '\0', strlen(oldinput));
}

/**
 * @brief Removes the input currently written on stdout and rewrites it.
 * 
 * @param input The current updated input.
 * @param cursor The position of the cursor in input.
 * @param len The length of the current input.
 * @param prompt The shell prompt so that it can be written to stdout.
 *
 * @note The shell prompt gets flushed out too. This is why it is 
 *       in the parameters to get rewritten.
 */
void redraw_line(char *input, int cursor, int len, char *prompt) {
    printf("\r\033[K");
    printf("%s", prompt);
    fwrite(input, 1, len, stdout);
    int move_back = len - cursor;
    if (move_back > 0) printf("\033[%dD", move_back);
    fflush(stdout);
}

/**
 * @brief Removes the current output, changes the input variable to the last 
 *        value (hist_index - 1) in the given history struct then rewrites it to stdout.
 *        
 * @param[out] input The input to be overwritten with the new history value.
 * @param hist_index The current index in history.
 * @param history The struct history that contains the current loaded history.
 * @param[out] cursor_pos The current cursor position. To be moved at the end of the new input.
 * @param prompt The shell prompt to be rewritten to stdout with the new input. 
 */
void go_back_hist(char *input, int *hist_index, struct history *history, int *cursor_pos, char *prompt) {
    if (*hist_index > 0) {
        remove_current_input(input);
        (*hist_index)--;
        memcpy(input, history->hist[*hist_index], strlen(history->hist[*hist_index]));
        *cursor_pos = strlen(input);
        printf("%s%s", prompt, input);
    }
}

/**
 * @brief Removes the current output, changes the input variable to the next
 *        value (hist_index + 1) in the given history struct then rewrites it to stdout.
 * 
 * @param[out] input The input to be overwritten with the new history value.
 * @param hist_index The current index in history.
 * @param history The struct history that contains the current loaded history.
 * @param[out] cursor_pos The current cursor position. To be moved at the end of the new input.
 * @param prompt The shell prompt to be rewritten to stdout with the new input. 
 */
void go_forward_hist(char *input, int *hist_index, struct history *history, int *cursor_pos, char *prompt) {
    if (*hist_index == history->length - 1) {
        remove_current_input(input);
        printf("%s", prompt);
        memset(input, '\0', MAX_INPUT);
        *cursor_pos = 0;
    } else if (*hist_index < history->length) {
        (*hist_index)++;
        remove_current_input(input);
        memcpy(input, history->hist[*hist_index], strlen(history->hist[*hist_index]));
        *cursor_pos = strlen(input);
        printf("%s%s", prompt, input);
    }
}

/**
 * @brief Moves the cursor left and printf the ANSI sequence to moves
 *         the cursor to the left too.
 * 
 * @param cursor_pos The current cursor position.
 */
void move_cursor_left(int *cursor_pos) {
    if (*cursor_pos > 0) {
        (*cursor_pos)--;
        printf("\033[D");
    }
}

/**
 * @brief Moves the cursor right and printf the ANSI sequence to moves
 *         the cursor to the right too.
 * 
 * @param cursor_pos The current cursor position.
 * @param current_input_size The size of the current input.
 */
void move_cursor_right(int *cursor_pos, int current_input_size) {
    if (*cursor_pos < current_input_size) {
        printf("\033[C");
        (*cursor_pos)++;
    }
}

/**
 * @brief Disables termios raw mode.
 * 
 */
void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/**
 * @brief Enables termios raw mode.
 * 
 */
void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/**
 * @brief My own getch (get char) function.
 * 
 * @return char The char that was input or the key that was pressed.
 *
 * @note This puts the terminal in raw mode using termios, then puts it back
 *       in normal mode when it has gotten the new character.
 * @note Some keys are made to be returned instead of characters if pressed.
 *       They are mapped inside the keys enum in inputs.h.
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

/**
 * @brief The main fgets() function used by the shell. It is my own little implementation.
 * 
 * @param[out] input The destination variable of the input.
 * @param[in] history The loaded history.
 * @param[in] prompt The current shell prompt.
 * @return int Returns 0 if it was exited with ENTER or 1 if CTRL-C.
 *
 * @note **It assumed input is an char array of size MAX_INPUT.**
 */
int snowshell_fgets(char *input, struct history *history, char *prompt) {
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
                go_back_hist(
                    input, 
                    &hist_index, 
                    history, 
                    &cursor,
                    prompt
                );
                input_length = cursor;
                break;
            case DOWN:
                go_forward_hist(
                    input, 
                    &hist_index, 
                    history, 
                    &cursor,
                    prompt
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
