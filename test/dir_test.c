#include <limits.h>
#include <criterion/criterion.h>
#include <stdlib.h>
#include <unistd.h>

#include "../src/dir.h"

void setup(void) { chdir("/"); }

Test(directory_testing, working_change_dir_to_home_directly, .init=setup) {
    char *argv[3] = {0};
    argv[0] = "cd";
    argv[1] = getenv("HOME");
    argv[2] = NULL;
    char *current_dir = "/";

    change_dir(argv, 2, current_dir);
    
    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)), "getcwd returned NULL.");

    cr_assert_str_eq(getenv("HOME"), real_current_dir, "The current directory isn't $HOME.");
}

Test(directory_testing, working_change_dir_to_home_no_args, .init=setup) {
    char *argv[2] = {0};
    argv[0] = "cd";
    argv[1] = NULL;
    char *current_dir = "/";

    change_dir(argv, 1, current_dir);
    
    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)), "getcwd returned NULL.");

    cr_assert_str_eq(getenv("HOME"), real_current_dir, "The current directory isn't $HOME.");
}

Test(directory_testing, working_change_to_root, .init=setup) {
    char *argv[3] = {0};
    argv[0] = "cd";
    argv[1] = "/";
    argv[2] = NULL;
    char *current_dir = "/";

    change_dir(argv, 2, current_dir);
    
    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)), "getcwd returned NULL.");

    cr_assert_str_eq("/", real_current_dir, "Current directory isn't root (/)");
}

Test(directory_testing, working_change_to_dir_without_absolute_path, .init=setup) {
    char *argv[3] = {0};
    argv[0] = "cd";
    argv[1] = "etc";
    argv[2] = NULL;
    char *current_dir = "/";

    change_dir(argv, 2, current_dir);
    
    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)), "getcwd returned NULL.");

    cr_assert_str_eq("/etc", real_current_dir, "Current directory isn't etc (/etc)");
}