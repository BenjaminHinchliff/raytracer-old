#ifndef INCLUDED_IMG_UTILS_H
#define INCLUDED_IMG_UTILS_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// NOTE: must be safe to cast to unsigned char**
// for writing
typedef struct RayColor {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RayColor;

// width must be greater than zero
// height must be greater than zero
// channels must be either 1 or 3
typedef struct RayImg {
  const int width;
  const int height;
  const int channels;
  RayColor *const *const pixels;
} RayImg;

// create a zero-initialized image with given specs
RayImg *ray_create_img(int width, int height, int channels);

void ray_free_img(RayImg *img);

void ray_set_pixel(int x, int y, const RayColor color, RayImg *img);

bool ray_png_write(char const *filename, const RayImg *img);

#endif // ifndef INCLUDED_IMG_UTILS_H