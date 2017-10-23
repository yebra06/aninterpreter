# aninterpreter
Unix command line interpreter in development.

## Environment
- Developed on macOS Sierra 10.12.6
- gcc -v Apple LLVM version 9.0.0 (clang-900.0.37)

## Instructions
- Type `make` to build program.
- Hit cntrl-c to exit or enter 'exit'.
- `./run`
- Type `make clean` to clean files.

### Example
```
aninterpreter (master) $ make
gcc -Wall -c main.c
gcc -o run main.o
aninterpreter (master) $ ./run

shhh> ls
Makefile	README.md	main.c		main.o		run

shhh> ^C
aninterpreter (master) $ make clean
rm *.o run
aninterpreter (master) $ ls
Makefile   README.md  main.c
aninterpreter (master) $
```
