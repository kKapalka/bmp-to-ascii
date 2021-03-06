#ifndef LIBRARY
#define LIBRARY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

#define BMP_CHECK 16973
//66*256+77
#define JPG_CHECK 65496
//255*256+216

#define WIDTH 1
#define HEIGHT 2
#define BYTES_PER_PIXEL 3

#define abort(a) fprintf(stderr,a,strerror(errno));return 1;


void assign_str(char **value, char *text);
void get_data(size_t *data, FILE *fp);
int assemble_bytes(FILE *fp, int size);
int init_pixelmap(unsigned char ***map, size_t *data); 
void fetch_pixelmap(FILE *fp, unsigned char **map, size_t *data);
void frender(FILE *fp, unsigned char **map,size_t *data);
void free_map(unsigned char **map, size_t *data);
int get_offset(FILE *fp);
int get_pixel(FILE *fp, int pixel_size);
#endif
