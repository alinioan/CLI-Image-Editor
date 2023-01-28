# Alexandru Alin-Ioan 312CA 2023

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

# define targets
TARGETS= program load free save actions

build: $(TARGETS) 
	$(CC) $(CFLAGS) main.o load.o free_memory.o save.o actions.o -lm -o image_editor

program:
	$(CC) $(CFLAGS) -c image_editor.c -o main.o

load:
	$(CC) $(CFLAGS) -c load.c -o load.o

free:
	$(CC) $(CFLAGS) -c free_memory.c -o free_memory.o

save:
	$(CC) $(CFLAGS) -c save.c -o save.o

actions:
	$(CC) $(CFLAGS) -c actions.c -o actions.o

clean:
	rm image_editor *.o
