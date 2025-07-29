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
     if (c == '\x1b') { // ESC
        char seq1 = getchar();
        char seq2 = getchar();

        if (seq1 == '[') {
            switch (seq2) {
                case 'A': 
                    c = UP; 
                    break; // Up
                case 'B': 
                    c = DOWN; 
                    break; // Down 
                case 'C': 
                    c = RIGHT; 
                    break; // Right 
                case 'D': 
                    c = LEFT; 
                    break; // Left
            }
        }
        // If not an arrow key, just return ESC
    } else if (c == 13) {
        c = ENTER_KEY;
    } else if (c == 127) {
        c = BACKSPACE;
    } else if (c == 3) {
        c = CTRL_C;
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
                    remove_current_input(input);
                    hist_index--;
                    memcpy(input, history->hist[hist_index], strlen(history->hist[hist_index]));
                    i = strlen(input) - 1;
                    printf("%s", input);
                }
                break;
            case DOWN:
                if (hist_index < history->length) {
                    remove_current_input(input);
                    hist_index++;
                    memcpy(input, history->hist[hist_index], strlen(history->hist[hist_index]));
                    i = strlen(input) - 1;
                    printf("%s", input);
                }
                break;
            case LEFT:
                printf("left");
                break;
            case RIGHT:
                printf("right");
                break;
            default:
                input[i] = c;
                printf("%c", c);
                break;
        }
    }

    return quit;
}
