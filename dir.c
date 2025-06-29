#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "dir.h"

void change_dir(char *args[], char *current_dir) {
    int success;

    if (args[1][0] == '/') {
        success = chdir(args[1]);
    } else {
        success = chdir(
            strcat(
                strcat(
                    current_dir,
                    "/"
                ),
                args[1]
            )
        );
    }

    if (success < 0) {
        switch (errno) {
            case ENOENT:
                printf("Directory %s does not exist\n", args[1]);
                break;
            case ENOTDIR:
                printf("%s is not a directory\n", args[1]);
                break;
        }
    }
}

void goto_home_dir() {
    chdir(getenv("HOME"));
}

