#ifndef DIR_H

/**
 * @brief Changes the current directory to the given one in argv.
 * 
 * @param argv The argv of cd. Like {"cd", "/home/user"}.
 * @param argc The argc of the given argv.
 * @param current_directory The absolute path to the current directory.
 */
void change_dir(char **argv, int argc, char *current_directory);

#endif
