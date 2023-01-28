// Copyright 2023 Alexandru Alin-Ioan 312CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_lib.h"

#define MAX_LINE_LEN 70

char *my_itoa(int val)
{
	static char buf[32] = {0};
	int i = 30;
	for (; val && i ; i--, val /= 10)
		buf[i] = "0123456789"[val % 10];
	return &buf[i + 1];
}

void write_txt(image img, FILE *fptr)
{
	int i, j;
	if (img.type == '2' || img.type == '5') {
		for (i = 0; i < img.height; i++) {
			for (j = 0; j < img.width; j++)
				fprintf(fptr, "%hhu ", img.gs_data[i][j]);
			fprintf(fptr, "\n");
		}
	} else {
		for (i = 0; i < img.height; i++) {
			for (j = 0; j < img.width; j++) {
				fprintf(fptr, "%hhu %hhu %hhu ",
						img.rgb_data[i][j].r,
						img.rgb_data[i][j].g,
						img.rgb_data[i][j].b);
			}
			fprintf(fptr, "\n");
		}
	}
}

void write_bin(image img, FILE *fptr)
{
	int i;
	if (img.type == '2' || img.type == '5') {
		for (i = 0; i < img.height; i++)
			fwrite(img.gs_data[i], sizeof(unsigned char), img.width, fptr);
	} else {
		for (i = 0; i < img.height; i++)
			fwrite(img.rgb_data[i], sizeof(struct pixel), img.width, fptr);
	}
}

void save(image img, char *argv[])
{
	if (img.type == 0) {
		printf("No image loaded\n");
		return;
	}
	if (!argv[0]) {
		printf("Invalid command\n");
		return;
	}

	char write_aux[MAX_LINE_LEN] = {'\0'};
	write_aux[0] = 'P';
	write_aux[1] = img.type;
	strcat(write_aux, "\n");
	strcat(write_aux, my_itoa(img.width));
	strcat(write_aux, " ");
	strcat(write_aux, my_itoa(img.height));
	strcat(write_aux, "\n");
	strcat(write_aux, my_itoa(img.maxval));
	strcat(write_aux, "\n");

	FILE *fptr;

	if (argv[1]) {
		fptr = fopen(argv[0], "wt");
		if (img.type == '5')
			write_aux[1] = '2';
		if (img.type == '6')
			write_aux[1] = '3';
		fprintf(fptr, "%s", write_aux);
		write_txt(img, fptr);
	} else {
		fptr = fopen(argv[0], "wb");
		if (img.type == '2')
			write_aux[1] = '5';
		if (img.type == '3')
			write_aux[1] = '6';
		fprintf(fptr, "%s", write_aux);
		write_bin(img, fptr);
	}
	printf("Saved %s\n", argv[0]);
	fclose(fptr);
}
