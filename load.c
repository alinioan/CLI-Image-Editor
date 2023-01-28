// Copyright 2023 Alexandru Alin-Ioan 312CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_lib.h"

#define MAX_LINE_LEN 70

// allocs memory for the grayscale image
unsigned char **alloc_gs_data(int m, int n)
{
	int i, j;
	unsigned char **a = (unsigned char **) malloc(sizeof(unsigned char *) * m);
	if (!a)
		return NULL;
	for (i = 0; i < m; i++) {
		a[i] = (unsigned char *) malloc(sizeof(unsigned char) * n);
		if (!a[i]) {
			// goes back and frees what has been allocated
			for (j = i - 1; j >= 0; j--)
				free(a[j]);
			free(a);
			return NULL;
		}
	}
	return a;
}

// allocs memory for the colour image
struct pixel **alloc_rgb_data(int m, int n)
{
	int i, j;
	struct pixel **a = (struct pixel **) malloc(sizeof(struct pixel *) * m);
	if (!a)
		return NULL;
	for (i = 0; i < m; i++) {
		a[i] = (struct pixel *) malloc(sizeof(struct pixel) * n);
		if (!a[i]) {
			// goes back and frees what has been allocated
			for (j = i - 1; j >= 0; j--)
				free(a[j]);
			free(a);
			return NULL;
		}
	}
	return a;
}

void skip_comments(FILE *fptr)
{
	char hash, line[MAX_LINE_LEN];
	hash = fgetc(fptr);
	hash = fgetc(fptr);
	while (hash == '#') {
		fscanf(fptr, "%[^\n]", line);
		hash = fgetc(fptr);
	}
	fseek(fptr, -1, SEEK_CUR);
}

void get_image_metadata(image *img, FILE *fptr)
{
	char magic_number[3], read_aux[MAX_LINE_LEN];
	// get magic number
	fgets(magic_number, 3, fptr);
	if (magic_number[0] != 'P') {
		img->type = 0;
		return;
	}
	if (magic_number[1] > '6' || magic_number[1] < '1') {
		img->type = 0;
		return;
	}
	img->type = magic_number[1];

	skip_comments(fptr);
	fscanf(fptr, "%s", read_aux);
	img->width = atoi(read_aux);

	skip_comments(fptr);
	fscanf(fptr, "%s", read_aux);
	img->height = atoi(read_aux);

	skip_comments(fptr);
	fscanf(fptr, "%s", read_aux);
	img->maxval = atoi(read_aux);

	// select all
	img->slct.x1 = 0;
	img->slct.y1 = 0;
	img->slct.x2 = img->width;
	img->slct.y2 = img->height;
}

// alloc memory for image
void alloc_data(image *img, int height, int width)
{
	// alloc memory for grayscale image
	if (img->type == '2' || img->type == '5') {
		img->rgb_data = NULL;
		img->gs_data = alloc_gs_data(height, width);
		if (!img->gs_data)
			return;
	}
	// alloc memory for rgb image
	if (img->type == '3' || img->type == '6') {
		img->gs_data = NULL;
		img->rgb_data = alloc_rgb_data(height, width);
		if (!img->rgb_data)
			return;
	}
}

// checks for image type and gets the coresponding data
void get_image_data(image *img, FILE *fptr)
{
	int i, j;
	alloc_data(img, img->height, img->width);
	if (!img->gs_data && !img->rgb_data)
		return;

	skip_comments(fptr);

	// get image frome file
	switch (img->type) {
	case '2':
		for (i = 0; i < img->height; i++)
			for (j = 0; j < img->width; j++)
				fscanf(fptr, "%hhu", &img->gs_data[i][j]);
		break;
	case '5':
		for (i = 0; i < img->height; i++)
			fread(img->gs_data[i], sizeof(unsigned char), img->width, fptr);
		break;
	case '3':
		for (i = 0; i < img->height; i++)
			for (j = 0; j < img->width; j++)
				fscanf(fptr, "%hhu %hhu %hhu",
					   &img->rgb_data[i][j].r,
					   &img->rgb_data[i][j].g,
					   &img->rgb_data[i][j].b);
		break;
	case '6':
		for (i = 0; i < img->height; i++)
			fread(img->rgb_data[i], sizeof(struct pixel), img->width, fptr);
		break;
	default:
		break;
	}
}

void load(image *img, char *argv[])
{
	if (!argv[0] || argv[1]) {
		printf("Invalid command\n");
		return;
	}
	FILE *fptr;
	if (img->type) {
		free_memory(img);
		img->type = 0;
	}
	fptr = fopen(argv[0], "r");
	if (!fptr) {
		printf("Failed to load %s\n", argv[0]);
		return;
	}

	get_image_metadata(img, fptr);
	if (img->type == 0) {
		fclose(fptr);
		printf("Failed to load %s\n", argv[0]);
		return;
	}

	get_image_data(img, fptr);
	if (!img->rgb_data && !img->gs_data) {
		fclose(fptr);
		printf("Failed to load %s\n", argv[0]);
		return;
	}

	fclose(fptr);
	printf("Loaded %s\n", argv[0]);
}
