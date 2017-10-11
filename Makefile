# Programmer: Alfredo Yebra Jr.
# 10/11/2017

all:
	gcc -Wall -O2 -c main.c
	gcc -o run main.o
	./run

clean:
	rm *.o run
