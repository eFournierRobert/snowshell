#include <errno.h>
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

static _Thread_local char hist_path[PATH_MAX];

void gen_hist_path() {
    char tmpl[PATH_MAX];
    snprintf(tmpl, sizeof(tmpl), "%s/.snowshell_history_XXXXXX", getenv("HOME"));

    int fd = mkstemp(tmpl);
    if (fd == -1) {
        fprintf(stderr, "mkstemp failed: %s\n", strerror(errno));
        abort();
    }

    strncpy(hist_path, tmpl, sizeof hist_path);
    hist_path[sizeof hist_path - 1] = '\0';

    close(fd); 
}

void setup(void) {
    // Change HOME to the current directory as to not ruin the current
    // tester homedir.
    char buf[PATH_MAX] = {0};
    memcpy(buf, "/tmp", PATH_MAX);

    setenv("HOME", buf, 1);
}

void full_history_file_setup(void) {
    // Change HOME to the current directory as to not ruin the current
    // tester homedir.
    setup();
    gen_hist_path();

    FILE *fptr;
    fptr = fopen(hist_path, "w");    

    //Dummy history
    for (int i = 0; i < 5; i++)
        fprintf(fptr, "Command: %d\n", i);

    fclose(fptr);
}

void teardown(void) {
    remove(hist_path);
}

Test (history_test_suite, get_hist_file_readptr_when_not_existing, .init=setup, .fini=teardown) {
    gen_hist_path();

    FILE *fptr = get_hist_file_readptr(hist_path);

    cr_assert_not_null(fptr, "Returned file pointer was null.");
    cr_assert_eq(access(hist_path, F_OK), 0, "Couldn't find %s", hist_path); // Make sure file was created
    fclose(fptr);
}

Test (history_test_suite, get_hist_file_readptr_when_existing, .init=setup, .fini=teardown) {
    gen_hist_path();

    // Create file
    FILE *fptr;
    fptr = fopen(hist_path, "w");
    fclose(fptr);

    fptr = get_hist_file_readptr(hist_path);

    cr_assert_not_null(fptr, "Returned file pointer was null.");
    fclose(fptr);
}

Test(history_test_suite, get_command_history_working, .init=full_history_file_setup, .fini=teardown) {
    history_t dest_hist = (history_t) {0};

    get_commands_history(&dest_hist, hist_path);

    for (int i = 0; i < 5; i++) {
        char expected[25];
        snprintf(expected, 25, "Command: %d", i);

        cr_assert_str_eq(dest_hist.hist[i], expected, "Command history doesn't match.\nExpected: %s\nGot: %s\nFrom hist_file: %s", expected, dest_hist.hist[i], hist_path);
    }

    cr_assert_eq(dest_hist.length, 5, "History length doesn't match.\nExpected: %d\nGot: %d", 5, dest_hist.length);
}

Test(history_test_suite, get_command_history_from_empty_history_file, .init=setup, .fini=teardown) {
    history_t dest_hist = (history_t) {0};
    gen_hist_path();

    get_commands_history(&dest_hist, hist_path);

    cr_assert_str_empty(dest_hist.hist[0], "First string of history is not empty when it is supposed to be.\nGot: %s from file: %s", dest_hist.hist[0], hist_path);
    cr_assert_eq(dest_hist.length, 0, "History length doesn't match.\nExpected: %d\nGot: %d", 0, dest_hist.length);
}

Test(history_test_suite, write_empty_history_to_file, .init=setup, .fini=teardown) {
    history_t hist_struct = (history_t) {0};
    gen_hist_path();

    write_hist(&hist_struct, hist_path);

    char file_first_line[10] = {0};

    FILE *fptr;
    fptr = fopen(hist_path, "r");

    fgets(file_first_line, 10, fptr);

    cr_assert_str_empty(file_first_line, "First line of history file was not empty when it was supposed to.\nGot: %s", file_first_line);

    fclose(fptr);
}

Test(history_test_suite, write_full_history_to_file, .init=setup, .fini=teardown) {
    int str_length = MAX_INPUT;
    
    history_t hist_struct = (history_t) {0};
    gen_hist_path();

    for (int i = 0; i < MAX_HIST_SIZE; i++) {
        char str[str_length];
        snprintf(str, str_length, "Command: %d", i);
        memcpy(hist_struct.hist[i], str, str_length);

        hist_struct.length++;
    }

    write_hist(&hist_struct, hist_path);

    char file_line[str_length];
    FILE *fptr;
    fptr = fopen(hist_path, "r");

    for (int i = 0; fgets(file_line, str_length, fptr); i++) {
        size_t len = strcspn(file_line, "\r\n");  // strip \n and optional \r
        file_line[len] = '\0';

        cr_assert_str_eq(hist_struct.hist[i], file_line, "Element inside history struct isn't equal to element inside history file.\nExpected: %s\nGot: %s\nFrom hist file: %s", hist_struct.hist[i], file_line, hist_path);
    }

    fclose(fptr);
}

Test(history_test_suite, push_to_not_full_hist, .init=setup, .fini=teardown) {
    int str_length = MAX_INPUT;
    
    history_t hist_struct = (history_t) {0};
    gen_hist_path();

    for (int i = 0; i < 100; i++) {
        char str[str_length];
        snprintf(str, str_length, "Command: %d", i);
        memcpy(hist_struct.hist[i], str, str_length);

        hist_struct.length++;
    }

    char expected[MAX_INPUT];
    strncpy(expected, "expected input", MAX_INPUT);

    push_to_hist(&hist_struct, expected);

    char *received = hist_struct.hist[hist_struct.length - 1];
    cr_assert_str_eq(received, expected, "Last stored input was not equal to the last input\nExpected: %s\nGot: %s", expected, received);
}

Test(history_test_suite, push_to_full_hist, .init=setup, .fini=teardown) {
    int str_length = MAX_INPUT;
    
    history_t hist_struct = (history_t) {0};
    gen_hist_path();

    for (int i = 0; i < MAX_HIST_SIZE; i++) {
        char str[str_length];
        snprintf(str, str_length, "Command: %d", i);
        push_to_hist(&hist_struct, str);
    }
    cr_assert_eq(MAX_HIST_SIZE, hist_struct.length);

    char expected[MAX_INPUT];
    strncpy(expected, "expected input", MAX_INPUT);

    push_to_hist(&hist_struct, expected);

    char *received = hist_struct.hist[hist_struct.length - 1];
    cr_assert_str_eq(received, expected, "Last stored input was not equal to the last input\nExpected: %s\nGot: %s", expected, received);
    cr_assert_str_eq("Command: 1", hist_struct.hist[0], "Commands where not or where badly shifted.\nExpected first command: Command 1\nGot: %s", hist_struct.hist[0]);
}