// Copyright 2023 Alexandru Alin-Ioan 312CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "image_lib.h"

#define MAX_VAL 256

void swap_ints(int *a, int *b)
{
	int aux = *a;
	*a = *b;
	*b = aux;
}

// checks if the arguments are numbers
// used for SELECT
int check_numbers(char *argv[], int argc)
{
	int i, j;
	for (i = 0; i < argc; i++) {
		if (argv[i][0] == '-')
			j = 1;
		else
			j = 0;
		for (; argv[i][j]; j++) {
			if (!isdigit(argv[i][j]))
				return 0;
		}
	}
	return 1;
}

// check if the location specified by the coordinates is within the image
int check_location(int x1, int y1, int x2, int y2, int width, int height)
{
	if (x1 < 0 || y1 < 0 || y2 < 0 || x2 < 0)
		return 0;
	if (x1 > width || x2 > width)
		return 0;
	if (y1 > height || y2 > height)
		return 0;
	if (x2 == x1 || y2 == y1)
		return 0;
	return 1;
}

void select_all(image *img)
{
	img->slct.x1 = 0;
	img->slct.y1 = 0;
	img->slct.x2 = img->width;
	img->slct.y2 = img->height;
}

void select(image *img, char *argv[])
{
	if (img->type == 0) {
		printf("No image loaded\n");
		return;
	}
	if (!argv[0]) {
		printf("Invalid command\n");
		return;
	}

	// this is for SELECT ALL
	if (strcmp(argv[0], "ALL") == 0) {
		if (argv[1]) {
			printf("Invalid command\n");
			return;
		}
		select_all(img);
		printf("Selected ALL\n");
		return;
	}

	if (!argv[1] || !argv[2] || !argv[3] || argv[4]) {
		printf("Invalid command\n");
		return;
	}

	if (!check_numbers(argv, 4)) {
		printf("Invalid command\n");
		return;
	}

	// makes the args ints
	int x1, y1, x2, y2;
	x1 = atoi(argv[0]);
	y1 = atoi(argv[1]);
	x2 = atoi(argv[2]);
	y2 = atoi(argv[3]);

	if (!check_location(x1, y1, x2, y2, img->width, img->height)) {
		printf("Invalid set of coordinates\n");
		return;
	}

	if (x1 > x2)
		swap_ints(&x1, &x2);
	if (y1 > y2)
		swap_ints(&y1, &y2);

	// place the args in the select zone
	img->slct.x1 = x1;
	img->slct.y1 = y1;
	img->slct.x2 = x2;
	img->slct.y2 = y2;
	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

// allocs memory for a new, smaller data matrix
void crop(image *img, char *argv[])
{
	if (argv[0]) {
		printf("Invalid command\n");
		return;
	}
	if (img->type == 0) {
		printf("No image loaded\n");
		return;
	}
	int i, j, rslti, rsltj;
	image rslt;
	rslt = *img;
	int slct_width = img->slct.x2 - img->slct.x1;
	int slct_height = img->slct.y2 - img->slct.y1;
	alloc_data(&rslt, slct_height, slct_width);
	if (!rslt.gs_data && !rslt.rgb_data) {
		printf("Memory error\n");
		return;
	}

	// gets the result from initial image
	for (i = img->slct.y1; i < img->slct.y2; i++) {
		for (j = img->slct.x1; j < img->slct.x2; j++) {
			rslti = i - img->slct.y1;
			rsltj = j - img->slct.x1;
			if (img->type == '2' || img->type == '5')
				rslt.gs_data[rslti][rsltj] = img->gs_data[i][j];
			else
				rslt.rgb_data[rslti][rsltj] = img->rgb_data[i][j];
		}
	}

	free_data(img, img->height);
	// places the resluts
	img->rgb_data = rslt.rgb_data;
	img->gs_data = rslt.gs_data;

	img->width = slct_width;
	img->height = slct_height;
	select_all(img);
	printf("Image cropped\n");
}

// places a rotatated image/selection in to a new image
void rotate90(image img, image *rslt)
{
	int i, j, rsltj, rslti;
	for (i = img.slct.y1; i < img.slct.y2; i++) {
		for (j = img.slct.x1; j < img.slct.x2; j++) {
			rslti = img.slct.y2 - 1 - i;
			rsltj = j - img.slct.x1;
			if (img.type == '2' || img.type == '5')
				rslt->gs_data[rsltj][rslti] = img.gs_data[i][j];
			else
				rslt->rgb_data[rsltj][rslti] = img.rgb_data[i][j];
		}
	}
}

// swaps the coresponding elements to rotate the image 180
void swap_180(image *img, int i, int j)
{
	int rslti, rsltj;
	if (img->slct.x2 - 1 - j >= 0) {
		rslti = (img->slct.y2 + img->slct.y1) - 1 - i;
		rsltj = (img->slct.x2 + img->slct.x1) - 1 - j;
		if (img->type == '2' || img->type == '5') {
			unsigned char aux = img->gs_data[i][j];
			img->gs_data[i][j] = img->gs_data[rslti][rsltj];
			img->gs_data[rslti][rsltj] = aux;
		} else {
			struct pixel aux = img->rgb_data[i][j];
			img->rgb_data[i][j] = img->rgb_data[rslti][rsltj];
			img->rgb_data[rslti][rsltj] = aux;
		}
	}
}

// instead of allocating a new image the roatation is made by swaping the
// the values in the original image
// this is possible because height and width don't change
void rotate180(image *img)
{
	int i, j;
	for (i = img->slct.y1; i < (img->slct.y2 + img->slct.y1) / 2; i++)
		for (j = img->slct.x1; j < (img->slct.x2 + img->slct.x1); j++)
			swap_180(img, i, j);
	if ((img->slct.y2 - img->slct.y1) % 2 == 1)
		for (j = img->slct.x1; j < (img->slct.x2 + img->slct.x1) / 2; j++)
			swap_180(img, (img->slct.y2 + img->slct.y1) / 2, j);
}

// places a rotatated image/selection in to a new image
void rotate270(image img, image *rslt)
{
	int i, j, rslti, rsltj;
	for (i = img.slct.y1; i < img.slct.y2; i++) {
		for (j = img.slct.x1; j < img.slct.x2; j++) {
			rsltj = img.slct.x2 - 1 - j;
			rslti = i - img.slct.y1;
			if (img.type == '2' || img.type == '5')
				rslt->gs_data[rsltj][rslti] = img.gs_data[i][j];
			else
				rslt->rgb_data[rsltj][rslti] = img.rgb_data[i][j];
		}
	}
}

// places the result(90 or 270 rotation) in the original image
void place_rslt(image *img, image rslt, int slct_height, int yes_slct)
{
	int i, j, rslti, rsltj;
	if (yes_slct == 0) {
		free_data(img, img->height);
		img->rgb_data = rslt.rgb_data;
		img->gs_data = rslt.gs_data;
		swap_ints(&img->height, &img->width);
		select_all(img);
	} else {
		// selection needs to be done element for element, because the result
		// uses the selection height and width
		for (i = img->slct.y1; i < img->slct.y2; i++) {
			for (j = img->slct.x1; j < img->slct.x2; j++) {
				rslti = i - img->slct.y1;
				rsltj = j - img->slct.x1;
				if (img->type == '2' || img->type == '5')
					img->gs_data[i][j] = rslt.gs_data[rslti][rsltj];
				else
					img->rgb_data[i][j] = rslt.rgb_data[rslti][rsltj];
			}
		}
		free_data(&rslt, slct_height);
	}
}

void rotate(image *img, char *argv[])
{
	image rslt;
	rslt.type = img->type;
	int angle = atoi(argv[0]), yes_slct = 0, rotation_complete = 1;
	int slct_width = img->slct.x2 - img->slct.x1;
	int slct_height = img->slct.y2 - img->slct.y1;

	if (img->type == 0) {
		printf("No image loaded\n");
		return;
	}
	// check if selection was made
	if (img->slct.x1 != 0 || img->slct.y1 != 0 ||
		img->slct.x2 != img->width || img->slct.y2 != img->height) {
		yes_slct = 1;
		if (slct_height != slct_width) {
			printf("The selection must be square\n");
			return;
		}
	}

	// some angles offer the same rotataion
	switch (angle) {
	case -270:
	case 90: {
		alloc_data(&rslt, slct_width, slct_height);
		if (!rslt.gs_data && !rslt.rgb_data) {
			printf("Memory error\n");
			return;
		}
		rotate90(*img, &rslt);
		place_rslt(img, rslt, slct_height, yes_slct);
	}
		break;
	case -180:
	case 180:
		rotate180(img);
		break;
	case -90:
	case 270: {
		alloc_data(&rslt, slct_width, slct_height);
		if (!rslt.gs_data && !rslt.rgb_data) {
			printf("Memory error\n");
			return;
		}
		rotate270(*img, &rslt);
		place_rslt(img, rslt, slct_height, yes_slct);
	}
		break;
	// this is the most glorious piece of code in the whole program
	case 0:
	case -360:
	case 360:
		break;
	default: {
		rotation_complete = 0;
		printf("Unsupported rotation angle\n");
	}
		break;
	}
	if (rotation_complete)
		printf("Rotated %d\n", angle);
}

void get_pixel_freq(image img, long freq[])
{
	int i, j;
	for (i = 0; i < img.height; i++)
		for (j = 0; j < img.width; j++)
			freq[img.gs_data[i][j]]++;
}

unsigned char clamp(int value)
{
	if (value < 0)
		value = 0;
	if (value > MAX_VAL - 1)
		value = MAX_VAL - 1;
	return value;
}

void histogram(image img, char *argv[])
{
	if (img.type == 0) {
		printf("No image loaded\n");
		return;
	}
	if (!argv[0] || !argv[1] || argv[2]) {
		printf("Invalid command\n");
		return;
	}
	if (img.type == '3' || img.type == '6') {
		printf("Black and white image needed\n");
		return;
	}

	int i, j;
	double max_ast_cnt, bin_cnt, *bin_value, max_freq = 0;
	double ast_cnt;
	long freq[MAX_VAL] = {0};

	max_ast_cnt = atoi(argv[0]);
	bin_cnt = atoi(argv[1]);

	bin_value = (double *)calloc(bin_cnt, sizeof(double));
	if (!bin_value) {
		printf("Memory error\n");
		return;
	}
	get_pixel_freq(img, freq);
	// calculate the value in each bin and the max bin value
	for (i = 0; i < bin_cnt; i++) {
		for (j = 0; j < MAX_VAL / bin_cnt; j++)
			bin_value[i] += freq[i * (int)(MAX_VAL / bin_cnt) + j];
		if (bin_value[i] > max_freq)
			max_freq = bin_value[i];
	}

	// prints the histogram
	for (i = 0; i < bin_cnt; i++) {
		// formula from documnet
		ast_cnt = trunc(bin_value[i] / max_freq * max_ast_cnt);
		printf("%d\t|\t", (int)ast_cnt);
		for (j = 0; j < ast_cnt; j++)
			printf("*");
		printf("\n");
	}

	free(bin_value);
}

void equalize(image img, char *argv[])
{
	if (img.type == 0) {
		printf("No image loaded\n");
		return;
	}
	if (img.type == '3' || img.type == '6') {
		printf("Black and white image needed\n");
		return;
	}
	if (argv[0]) {
		printf("Invalid command\n");
		return;
	}
	int i, j;
	long freq[MAX_VAL] = {0};
	double sum_freq[MAX_VAL] = {0.0}, result;
	double area = img.width * img.height;
	get_pixel_freq(img, freq);
	sum_freq[0] = freq[0];
	for (i = 1; i < MAX_VAL; i++)
		sum_freq[i] = sum_freq[i - 1] + freq[i];
	// equalize is applied here
	for (i = 0; i < img.height; i++) {
		for (j = 0; j < img.width; j++) {
			// formula from document
			result = (255.0 / area) * sum_freq[img.gs_data[i][j]];
			img.gs_data[i][j] = clamp(round(result));
		}
	}
	printf("Equalize done\n");
}

// makes the apply zone ignor the pixels on the edge
void get_valid_zone(struct zone *apply_zone, int height, int width)
{
	if (apply_zone->x1 == 0)
		apply_zone->x1++;
	if (apply_zone->y1 == 0)
		apply_zone->y1++;
	if (apply_zone->x2 == width)
		apply_zone->x2--;
	if (apply_zone->y2 == height)
		apply_zone->y2--;
}

void apply_filter(image *img, struct pixel **rslt,
				  struct zone apply_zone, double krnl[][3])
{
	int i, j, k, l;
	double sumr, sumg, sumb;
	for (i = apply_zone.y1; i < apply_zone.y2; i++)
		for (j = apply_zone.x1; j < apply_zone.x2; j++) {
			sumr = 0.0;
			sumg = 0.0;
			sumb = 0.0;
			// we go through each kernel elemnt and calculate the sums
			for (k = 0; k < 3; k++) {
				for (l = 0; l < 3; l++) {
					sumr += krnl[k][l] * img->rgb_data[i - 1 + k][j - 1 + l].r;
					sumg += krnl[k][l] * img->rgb_data[i - 1 + k][j - 1 + l].g;
					sumb += krnl[k][l] * img->rgb_data[i - 1 + k][j - 1 + l].b;
				}
			}
			// place the result
			rslt[i - apply_zone.y1][j - apply_zone.x1].r = clamp(round(sumr));
			rslt[i - apply_zone.y1][j - apply_zone.x1].g = clamp(round(sumg));
			rslt[i - apply_zone.y1][j - apply_zone.x1].b = clamp(round(sumb));
		}
}

void apply(image *img, char *argv[])
{
	if (img->type == 0) {
		printf("No image loaded\n");
		return;
	}
	if (!argv[0] || argv[1]) {
		printf("Invalid command\n");
		return;
	}
	if (img->type == '2' || img->type == '5') {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	// kernel declarations
	double krnl_edge[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
	double krnl_sharpen[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
	double krnl_blur[3][3] = {{1.0 / 9, 1.0 / 9, 1.0 / 9},
							  {1.0 / 9, 1.0 / 9, 1.0 / 9},
							  {1.0 / 9, 1.0 / 9, 1.0 / 9}};
	double krnl_g_blur[3][3] = {{1.0 / 16, 2.0 / 16, 1.0 / 16},
								{2.0 / 16, 4.0 / 16, 2.0 / 16},
								{1.0 / 16, 2.0 / 16, 1.0 / 16}};
	image rslt;
	rslt.type = img->type;
	int i, j, slct_height, slct_width, apply_done = 1;
	struct zone apply_zone;

	// select zone could be invalid so we need a new one
	apply_zone.x1 = img->slct.x1;
	apply_zone.x2 = img->slct.x2;
	apply_zone.y1 = img->slct.y1;
	apply_zone.y2 = img->slct.y2;

	get_valid_zone(&apply_zone, img->height, img->width);
	slct_width = apply_zone.x2 - apply_zone.x1;
	slct_height = apply_zone.y2 - apply_zone.y1;

	alloc_data(&rslt, slct_height, slct_width);
	if (!rslt.gs_data && !rslt.rgb_data) {
		printf("Memory error\n");
		return;
	}

	// init the result
	// maybe could have used memset
	for (i = 0; i < slct_height; i++)
		for (j = 0; j < slct_width; j++) {
			rslt.rgb_data[i][j].r = 0;
			rslt.rgb_data[i][j].g = 0;
			rslt.rgb_data[i][j].b = 0;
		}

	// command parser 2.0?
	if (strcmp(argv[0], "EDGE") == 0) {
		apply_filter(img, rslt.rgb_data, apply_zone, krnl_edge);
	} else if (strcmp(argv[0], "SHARPEN") == 0) {
		apply_filter(img, rslt.rgb_data, apply_zone, krnl_sharpen);
	} else if (strcmp(argv[0], "BLUR") == 0) {
		apply_filter(img, rslt.rgb_data, apply_zone, krnl_blur);
	} else if (strcmp(argv[0], "GAUSSIAN_BLUR") == 0) {
		apply_filter(img, rslt.rgb_data, apply_zone, krnl_g_blur);
	} else {
		apply_done = 0;
		printf("APPLY parameter invalid\n");
	}

	// places result in the apply zone
	if (apply_done) {
		for (i = apply_zone.y1; i < apply_zone.y2; i++)
			for (j = apply_zone.x1; j < apply_zone.x2; j++)
				img->rgb_data[i][j] =
				rslt.rgb_data[i - apply_zone.y1][j - apply_zone.x1];
		printf("APPLY %s done\n", argv[0]);
	}
	free_data(&rslt, slct_height);
}
