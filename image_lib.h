// Copyright 2023 Alexandru Alin-Ioan 312CA
#ifndef image_lib
	#define image_lib

struct pixel {
	unsigned char r, g, b;
};

// the selection coords uses this
struct zone {
	int x1, y1;
	int x2, y2;
};

typedef struct image_struct {
	char type;
	unsigned char maxval;
	int width, height;
	// values used for colour format
	struct pixel **rgb_data;
	// values used for grayscale format
	unsigned char **gs_data;

	struct zone slct;
} image;

void swap_ints(int *a, int *b);

// load.c functions
void load(image *img, char *argv[]);

unsigned char **alloc_gs_data(int m, int n);

struct pixel **alloc_rgb_data(int m, int n);

void alloc_data(image *img, int height, int width);

// save.c function
void save(image img, char *argv[]);

// free_memory.c functions
void free_memory(image *img);

void free_gs_data(unsigned char **a, int m);

void free_rgb_data(struct pixel **a, int m);

void free_data(image *img, int height);

// actions.c functions
void select(image *img, char *argv[]);

void crop(image *img, char *argv[]);

void rotate(image *img, char *argv[]);

void histogram(image img, char *argv[]);

void equalize(image img, char *argv[]);

void apply(image *img, char *argv[]);

#endif
