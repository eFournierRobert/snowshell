# Snowshell

Minimal toy shell written in C built for fun and to expand my C/systems skills.

## Compile and run
This project uses CMake, so I would highly recommend that you install it if you
haven't already. It doesn't use any external libraries, so you just need a C compiler.
The project was tested with both GCC and Clang.

You can run this command and it will compile the project and run it:
```bash
mkdir build && cd build && cmake .. && cmake --build . --target run
```

A release build can be compiled using (assuming you already generated the Makefile):
```bash
cd build && cmake --build . --config Release
```

Shell and man pages can be installed using:
```bash
sudo ./install.sh
```

Then it can be uninstalled using:
```bash
sudo ./uninstall.sh
```

## Current features
Snowshell currently supports basic things you would expect from
a rudimentary shell like:
- Execute programs
- Go through directories
- Has a history of past commands
- Pipe commands together

Small demo:
```bash
$ ./snowshell 
[ /home/elliott/Documents/snowshell/build ]-> cd
[ /home/elliott ]-> mkdir test
[ /home/elliott ]-> cd test
[ /home/elliott/test ]-> nvim test.py
[ /home/elliott/test ]-> python test.py
Hello World!
[ /home/elliott/test ]-> exit
```

Pipe demo:
```bash
[ /home/elliott/Documents/snowshell ]-> cat README.md | tr 'a-z' 'A-Z' | head -n 4
# SNOWSHELL

SMALL AND SIMPLE SHELL MADE IN C. JUST SOMETHING I MADE FOR FUN AND 
TO LEARN MORE ABOUT THE LANGUAGE.
```

## Running tests
If you want to run the unit tests, you can do this:
```bash
cd build && cmake --build . && ctest --output-on-failure
```

## Future improvements
I think the shell is pretty much where I'd like it to be, but here's some 
ideas that could be worked on:

- Better C practices and just general refactoring of the code as I get 
better with it
- Simple redirection (like ls -l > output.txt) could be fun
- Adding the '&&' command