#include <criterion/criterion.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../src/dir.h"

char test_filename[10] = "testfile";

void setup(void) { chdir("/"); }

void not_dir_setup(void) {
    FILE *fptr = fopen(test_filename, "w");
    fclose(fptr);
}

void not_dir_teardown(void) { remove(test_filename); }

Test(directory_testing, working_change_dir_to_home_directly, .init = setup) {
    char *argv[3] = {0};
    argv[0] = "cd";
    argv[1] = getenv("HOME");
    argv[2] = NULL;
    char *current_dir = "/";

    change_dir(argv, 2, current_dir);

    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)),
                       "getcwd returned NULL.");

    cr_assert_str_eq(getenv("HOME"), real_current_dir,
                     "The current directory isn't $HOME.");
}

Test(directory_testing, working_change_dir_to_home_no_args, .init = setup) {
    char *argv[2] = {0};
    argv[0] = "cd";
    argv[1] = NULL;
    char *current_dir = "/";

    change_dir(argv, 1, current_dir);

    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)),
                       "getcwd returned NULL.");

    cr_assert_str_eq(getenv("HOME"), real_current_dir,
                     "The current directory isn't $HOME.");
}

Test(directory_testing, working_change_to_root, .init = setup) {
    char *argv[3] = {0};
    argv[0] = "cd";
    argv[1] = "/";
    argv[2] = NULL;
    char *current_dir = "/";

    change_dir(argv, 2, current_dir);

    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)),
                       "getcwd returned NULL.");

    cr_assert_str_eq("/", real_current_dir, "Current directory isn't root (/)");
}

Test(directory_testing, working_change_to_dir_without_absolute_path,
     .init = setup) {
    char *argv[3] = {0};
    argv[0] = "cd";
    argv[1] = "etc";
    argv[2] = NULL;
    char *current_dir = "/";

    change_dir(argv, 2, current_dir);

    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)),
                       "getcwd returned NULL.");

    cr_assert_str_eq("/etc", real_current_dir,
                     "Current directory isn't etc (/etc)");
}

Test(directory_testing, change_to_non_existant_dir, .init = setup) {
    char *argv[3] = {0};
    argv[0] = "cd";
    argv[1] = "doesnotexist";
    argv[2] = NULL;
    char *current_dir = "/";

    char buf[100];
    setbuf(stderr, buf);

    char expected[100];
    snprintf(expected, 100, "Directory %s does not exist\n", argv[1]);

    change_dir(argv, 2, current_dir);

    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)),
                       "getcwd returned NULL.");

    cr_assert_str_eq(buf, expected,
                     "Error message did not match the expected error message.");
    cr_assert_str_eq(current_dir, real_current_dir,
                     "Directory shouldn't have change on invalid directory");
}

Test(directory_testing, change_to_a_file, .init = not_dir_setup,
     .fini = not_dir_teardown) {
    char *argv[3] = {0};
    argv[0] = "cd";
    argv[1] = test_filename;
    argv[2] = NULL;
    char current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(current_dir, sizeof(current_dir)),
                       "getcwd returned NULL.");

    char buf[30] = {0};
    setbuf(stderr, buf);

    char expected[30] = {0};
    snprintf(expected, 30, "%s is not a directory\n", argv[1]);

    change_dir(argv, 2, current_dir);

    cr_assert_str_eq(buf, expected,
                     "Error message did not match the expected error "
                     "message.\nExpected: %s\nGot: %s",
                     expected, buf);
}