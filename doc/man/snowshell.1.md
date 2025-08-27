---
title: snowshell
section: 1
header: Minimal toy shell
footer: snowshell
date: 2025-08-27
---

# snowhell (1)

## NAME
snowshell - minimal toy shell

## SYNOPSIS
**snowshell** [options]

## COPYRIGHT
snowshell is under MIT license. Check the repository on GitHub.

## DESCRIPTION
Snowshell is a minimal shell made to expand my C/systems skills. It isn't POSIX, nor does it aim to be It is made to remain simple and somewhat barebone.

## AUTHOR
Snowshell was made and is maintained by Elliott Fournier-Robert **<elliott.fournierrobert@gmail.com>**

## BUILTINS
*cd* [DIR]\
&nbsp;&nbsp;&nbsp;&nbsp;Change directory. If [dir] is empty, it goes to $HOME.

*history*\
&nbsp;&nbsp;&nbsp;&nbsp;Prints the current loaded history.

*exit*\
&nbsp;&nbsp;&nbsp;&nbsp;Saves the current command history to the history file, then exit the shell.

## PIPES
Use *|* to connect commands: ```ls | wc -l```.

## FILES
**$HOME/.snowshell_history** for command history

## EXIT STATUS
0 on success, nonzero on error.

## SEE ALSO
snowhsell-language(7)