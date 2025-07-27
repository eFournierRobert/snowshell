#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "inputs.h"

struct termios orig_termios;

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
    disable_raw_mode();
    return c;
}

int snowshell_fgets(char *input) {
    int quit = -1;

    memset(input, '\0', sizeof(input));

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
                input[i - 1] = '\0';
                printf("\b \b");
                i -= 2;
                break;
            default:
                input[i] = c;
                printf("%c", c);
                break;
        }
    }

    return quit;
}
