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