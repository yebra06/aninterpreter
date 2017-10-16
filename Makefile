# Programmer: Alfredo Yebra Jr.
# 10/11/2017

all:
	gcc -Wall -c src/main.c
	gcc -o run main.o

clean:
	rm src/*.o run
