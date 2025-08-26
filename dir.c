#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dir.h"

/**
 * @brief Change directory to the directory mentionned in $HOME.
 */
void goto_home_dir() { chdir(getenv("HOME")); }

/**
 * @brief Change to the given directory in args[1].
 *
 * @param args Arguments to change directory (like {"cd", "home"}).
 * @param argc The amount of arguments in @p args.
 * @param current_dir The absolute path of the current directory.
 */
void change_dir(char *args[], int argc, char *current_dir) {
    int success;

    if (argc == 1) {
        goto_home_dir();
        return;
    }

    if (argc == 2 && args[1][0] == '/')
        success = chdir(args[1]);
    else {
        int path_size = strlen(current_dir) + strlen(args[1]) + 2;
        char path[path_size];

        snprintf(path, path_size, "%s/%s", current_dir, args[1]);
        success = chdir(path);
    }

    if (success < 0) {
        switch (errno) {
        case ENOENT:
            fprintf(stderr, "Directory %s does not exist\n", args[1]);
            break;
        case ENOTDIR:
            fprintf(stderr, "%s is not a directory\n", args[1]);
            break;
        default:
            fprintf(stderr, "error changing to directory %s: %s", args[1],
                    strerror(errno));
            break;
        }
    }
}
