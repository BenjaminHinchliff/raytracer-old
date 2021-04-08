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

#include "ray/img_utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "png.h" // png_*

#include "ray/vec_utils.h"

RayImg *ray_read_img(const char *path) {
  FILE *infile = fopen(path, "rb");
  if (infile == NULL) {
    fprintf(stderr, "failed to open file \"%s\"\n", path);
    return NULL;
  }

  unsigned char sig[8];
  fread(sig, 1, 8, infile);
  if (!png_check_sig(sig, 8)) {
    fprintf(stderr, "file passed to read_img wasn't a png (or is corrupted)\n");
    return NULL;
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fprintf(stderr, "initialization of libpng failed\n");
    return NULL;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fprintf(stderr, "initiliazation if info struct failed\n");
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return NULL;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "something went wrong while reading png\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return NULL;
  }

  png_init_io(png_ptr, infile);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);

  unsigned int uwidth;
  unsigned int uheight;
  int bit_depth;
  int color_type;
  png_get_IHDR(png_ptr, info_ptr, &uwidth, &uheight, &bit_depth, &color_type, NULL, NULL, NULL);
  assert(bit_depth == 8);
  assert(uwidth <= INT_MAX);
  assert(uheight <= INT_MAX);
  assert(color_type == PNG_COLOR_TYPE_RGB && "color type must be rgb (no alpha)");

  int width = (int)uwidth;
  int height = (int)uheight;
  int channels = 3;

  RayImg *img = ray_create_img(width, height, channels);
  for (int y = 0; y < height; ++y) {
    png_bytep row = malloc(width * channels * (sizeof *row));
    png_read_row(png_ptr, row, NULL);
    for (int x = 0; x < width; ++x) {
      int row_x = x * channels;
      double r = (double)row[row_x] / UCHAR_MAX;
      double g = (double)row[row_x + 1] / UCHAR_MAX;
      double b = (double)row[row_x + 2] / UCHAR_MAX;
      img->pixels[y][x] = ray_create_vec3(r, g, b);
    }
    free(row);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  fclose(infile);

  return img;
}

RayImg *ray_create_img(int width, int height, int channels) {
  gsl_vector ***pixels = malloc(height * (sizeof *pixels));
  for (int y = 0; y < height; y += 1) {
    pixels[y] = malloc(width * (sizeof *pixels[y]));
    for (int x = 0; x < width; x += 1) {
      pixels[y][x] = NULL;
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
    for (int x = 0; x < img->width; x += 1) {
      gsl_vector_free(img->pixels[y][x]);
    }
    free(img->pixels[y]);
  }
  free((void *)img->pixels);
  free(img);
}

void ray_set_pixel(int x, int y, gsl_vector *color, RayImg *img) {
  assert(color->size == img->channels &&
         "the number of components in the vector and channels must be equal");
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

  for (int y = 0; y < img->height; ++y) {
    png_bytep row = malloc(img->width * img->channels * (sizeof *row));
    for (int x = 0; x < img->width; ++x) {
      gsl_vector *color = img->pixels[y][x];
      assert(color != NULL && "null vector in image");
      int row_x = img->channels * x;
      row[row_x] = (unsigned char)(gsl_vector_get(color, 0) * UCHAR_MAX);
      row[row_x + 1] = (unsigned char)(gsl_vector_get(color, 1) * UCHAR_MAX);
      row[row_x + 2] = (unsigned char)(gsl_vector_get(color, 2) * UCHAR_MAX);
    }
    png_write_row(png_ptr, row);
    free(row);
  }

  png_write_end(png_ptr, NULL);

  // cleanup
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(outfile);

  return true;
}
