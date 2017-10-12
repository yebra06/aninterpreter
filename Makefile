# Programmer: Alfredo Yebra Jr.
# 10/11/2017

all:
	gcc -Wall -c main.c
	gcc -o run main.o

clean:
	rm *.o run
