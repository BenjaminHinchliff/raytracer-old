//  a small and simple raytracer
//  Copyright (C) 2021  Benjamin Hinchliff
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//  USA

#include "img_utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "png.h" // png_*

RayImg *ray_create_img(int width, int height, int channels) {
  RayColor **pixels = malloc(height * (sizeof *pixels));
  for (int y = 0; y < height; y += 1) {
    pixels[y] = malloc(width * (sizeof *pixels[y]));
    for (int x = 0; x < width; x += 1) {
      pixels[y][x] = (RayColor){0, 0, 0};
    }
  }
  RayImg *img = malloc(sizeof *img);
  RayImg stack_img = {
      .width = width,
      .height = height,
      .channels = channels,
      .pixels = pixels,
  };
  memcpy(img, &stack_img, sizeof stack_img);
  return img;
}

void ray_free_img(RayImg *img) {
  for (int y = 0; y < img->height; y += 1) {
    free(img->pixels[y]);
  }
  free((void *)img->pixels);
  free(img);
}

void ray_set_pixel(int x, int y, const RayColor color, RayImg *img) {
  assert(x >= 0 && "x cannot be negative");
  assert(x < img->width && "x must be less than the image width");
  assert(y >= 0 && "y cannot be negative");
  assert(y < img->height && "y must be less than the image height");
  img->pixels[y][x] = color;
}

bool ray_png_write(char const *filename, const RayImg *img) {
  assert(filename != NULL && "filename cannot be null");
  FILE *outfile = fopen(filename, "wb");
  if (outfile == NULL) {
    fprintf(stderr, "failed to open file \"%s\"\n", filename);
    return false;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(outfile);
    fprintf(stderr, "png_create_write_struct failed\n");
    return false;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(outfile);
    fprintf(stderr, "png_create_info_struct failed\n");
    return false;
  }

  png_init_io(png_ptr, outfile);

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(outfile);
    fprintf(stderr, "error during writing\n");
    return false;
  }

  png_set_IHDR(png_ptr, info_ptr, img->width, img->height, 8,
               img->channels == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  png_write_image(png_ptr, (unsigned char **)img->pixels);

  png_write_end(png_ptr, NULL);

  // cleanup
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(outfile);

  return true;
}
