cmake_minimum_required(VERSION 4.0)
project(snowshell C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

add_executable(snowshell 
        main.c
        dir.c
        history.c
        inputs.c
        execute.c
)

target_compile_options(snowshell PRIVATE -Wall -Wextra -Werror -g)