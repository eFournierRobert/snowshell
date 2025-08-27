# Snowshell Dev Doc - Overview

## Purpose 
Snowshell is a toy shell made in C. It is a small project to get me more comfortable with C and getting
closer to the OS. This is more of a personal project of mine and not something I think should be deployed
to a production environment.

It is rudimentary so it is not POSIX compliant nor does it aim to be.

## High-Level Architecture
The main loop is basically a REPL (print prompt -> read line -> parse -> execute -> repeat) shell.

The parser splits uses ```wordexp(3)``` to perform lexical analysis, splitting input into tokens
on whitespaces unless quoted. Pipelines are recognized and splitted on the pipe ('|') character, 
then each command is parsed and executed sequentially.

When executing commands, it makes a basic fork + execvp call. The parent (shell) will wait for the 
children to finish.

Here is the list of builtins:
- exit\
    Exits the terminal
- cd\
    Changes directory. If it has no arguments, it will go to ```$HOME```.
- history\
    Prints the current loaded command history with the index.

The command history is loaded into memory at startup, then kept in memory until the ```exit``` or it gets
a signal (like SIGHUP). That's when it will save it to the history file.

## Current limitations
- No escaping or expansion
- No redirection
- No job control
- No command chaining