#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
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