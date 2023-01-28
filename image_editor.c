// Copyright 2023 Alexandru Alin-Ioan 312CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_lib.h"

#define CMD_MAX_SIZE 300
#define MAX_ARGC 5
#define TRUE 1

// reads the entire command with its argumnents from input
void read_command(char **command)
{
	int cmd_len;
	fgets(*command, sizeof(char) * CMD_MAX_SIZE, stdin);
	// removes the '\n' character when reading the command
	(*command)[strcspn(*command, "\n")] = 0;
	cmd_len = strlen(*command);
	*command = (char *)realloc(*command, cmd_len + 1);
	// realloc will always alloc less memory (cmd_len < CMD_MAX_SIZE)
	// so no defense needed
}

// splits the arguments into multiple one word strings
void get_arguments(char *my_argv[MAX_ARGC], int *my_argc)
{
	int i = 0;
	my_argv[i++] = strtok(NULL, " ");
	while (my_argv[i - 1])
		my_argv[i++] = strtok(NULL, " ");
	*my_argc = i - 1;
}

int main(void)
{
	image img;
	img.type = 0;
	int my_argc = 0;
	char *command_line, *command, *my_argv[MAX_ARGC] = {NULL};
	int exit_check = 0;

	while (exit_check == 0) {
		command_line = (char *)malloc(CMD_MAX_SIZE * sizeof(char));
		if (!command_line) {
			printf("Command failed!");
			return 1;
		}
		read_command(&command_line);

		// checks if it recived only a '\n'
		if (command_line[0] == '\0') {
			free(command_line);
			continue;
		}

		// get the main command and its args
		command = strtok(command_line, " ");
		get_arguments(my_argv, &my_argc);

		// command parser?
		if (strcmp(command, "LOAD") == 0) {
			load(&img, my_argv);
		} else if (strcmp(command, "SELECT") == 0) {
			// SELECT ALL is also in here
			select(&img, my_argv);
		} else if (strcmp(command, "HISTOGRAM") == 0) {
			histogram(img, my_argv);
		} else if (strcmp(command, "EQUALIZE") == 0) {
			equalize(img, my_argv);
		} else if (strcmp(command, "ROTATE") == 0) {
			rotate(&img, my_argv);
		} else if (strcmp(command, "CROP") == 0) {
			crop(&img, my_argv);
		} else if (strcmp(command, "APPLY") == 0) {
			apply(&img, my_argv);
		} else if (strcmp(command, "SAVE") == 0) {
			save(img, my_argv);
		} else if (strcmp(command, "EXIT") == 0) {
			if (!my_argv[0]) {
				free_memory(&img);
				exit_check = 1;
			}
		} else {
			printf("Invalid command\n");
		}
		free(command_line);
	}
	return 0;
}
