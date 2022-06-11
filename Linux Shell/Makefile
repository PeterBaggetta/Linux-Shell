CC = gcc
FLAGS = -Wpedantic -std=gnu99

all: myShell.o myShell

myShell.o: myShell.c myHeader.h
	$(CC) $(FLAGS) -c myShell.c -o myShell.o
myShell: myShell.o 
	$(CC) $(FLAGS) myShell.o -o myShell

clean:
	rm -i *.o myShell