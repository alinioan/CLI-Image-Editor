// Copyright 2023 Alexandru Alin-Ioan 312CA
#include <stdio.h>
#include <stdlib.h>
#include "image_lib.h"

void free_gs_data(unsigned char **a, int m)
{
	int i;
	for (i = 0; i < m; i++)
		free(a[i]);
	free(a);
}

void free_rgb_data(struct pixel **a, int m)
{
	int i;
	for (i = 0; i < m; i++)
		free(a[i]);
	free(a);
}

void free_data(image *img, int height)
{
	if (img->gs_data)
		free_gs_data(img->gs_data, height);
	if (img->rgb_data)
		free_rgb_data(img->rgb_data, height);
}

void free_memory(image *img)
{
	if (img->type == 0) {
		printf("No image loaded\n");
		return;
	}
	free_data(img, img->height);
}
