#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Just for the LSP
#ifndef TESTING
    #define TESTING
#endif
#include "../src/history.h"

void setup(void) {
    // Change HOME to the current directory as to not ruin the current
    // tester homedir.
    char buf[PATH_MAX];
    getcwd(buf, sizeof(buf));

    setenv("HOME", buf, 1);
}

void full_history_file_setup(void) {
    // Change HOME to the current directory as to not ruin the current
    // tester homedir.
    setup();

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/.snowshell_history", getenv("HOME"));

    FILE *fptr;
    fptr = fopen(path, "w");    

    //Dummy history
    for (int i = 0; i < 5; i++)
        fprintf(fptr, "Command: %d\n", i);

    fclose(fptr);
}

void teardown(void) {
    char buf[PATH_MAX] = {0};
    snprintf(buf, sizeof(buf), "%s/.snowshell_history", getenv("HOME"));
    remove(buf);
}

Test (history_test_suite, get_hist_file_readptr_when_not_existing, .init=setup, .fini=teardown) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/.snowshell_history", getenv("HOME"));

    FILE *fptr = get_hist_file_readptr();

    cr_assert_not_null(fptr, "Returned file pointer was null.");
    cr_assert_eq(access(path, F_OK), 0, "Couldn't find %s", path); // Make sure file was created
    fclose(fptr);
}

Test (history_test_suite, get_hist_file_readptr_when_existing, .init=setup, .fini=teardown) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/.snowshell_history", getenv("HOME"));

    // Create file
    FILE *fptr;
    fptr = fopen(path, "w");
    fclose(fptr);

    fptr = get_hist_file_readptr();

    cr_assert_not_null(fptr, "Returned file pointer was null.");
    fclose(fptr);
}

Test(history_test_suite, get_command_history_working, .init=full_history_file_setup, .fini=teardown) {
    history_t dest_hist;
    dest_hist.length = 0;
    char history[MAX_HIST_SIZE][MAX_INPUT];
    memcpy(dest_hist.hist, history, MAX_HIST_SIZE);

    get_commands_history(&dest_hist);

    for (int i = 0; i < 5; i++) {
        char str[25];
        snprintf(str, 25, "Command: %d", i);

        cr_assert_str_eq(dest_hist.hist[i], str, "Command history doesn't match.\nExpected: %s\nGot: %s", str, dest_hist.hist[i]);
    }

    cr_assert_eq(dest_hist.length, 5, "History length doesn't match.\nExpected: %d\nGot: %d", 5, dest_hist.length);
}

Test(history_test_suite, get_command_history_from_empty_history_file, .init=setup, .fini=teardown) {
    history_t dest_hist;
    dest_hist.length = 0;
    char history[MAX_HIST_SIZE][MAX_INPUT];
    memcpy(dest_hist.hist, history, MAX_HIST_SIZE);

    get_commands_history(&dest_hist);

    cr_assert_str_empty(dest_hist.hist[0], "First string of history is not empty when it is supposed to be.");
    cr_assert_eq(dest_hist.length, 0, "History length doesn't match.\nExpected: %d\nGot: %d", 5, dest_hist.length);
}