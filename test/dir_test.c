#include <limits.h>
#include <criterion/criterion.h>
#include <stdlib.h>
#include <unistd.h>

#include "../src/dir.h"

void setup(void) { chdir("/"); }

Test(directory_testing, working_change_dir_to_home, .init=setup) {
    char *argv[2] = {0};
    argv[0] = "cd";
    argv[1] = getenv("HOME");
    char *current_dir = "/";

    change_dir(argv, 2, current_dir);
    
    char real_current_dir[PATH_MAX];
    cr_assert_not_null(getcwd(real_current_dir, sizeof(real_current_dir)), "getcwd returned NULL.");

    cr_assert_str_eq(getenv("HOME"), real_current_dir, "current_dir and real_current_dir are not equal.");
}