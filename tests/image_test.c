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
#include "ray/vec_utils.h"

#define IMG_WIDTH 50
#define IMG_HEIGHT 20
#define IMG_CHANNELS 3

int main() {
  RayImg *img = ray_create_img(IMG_WIDTH, IMG_HEIGHT, IMG_CHANNELS);
  for (int y = 0; y < IMG_HEIGHT; y += 1) {
    for (int x = 0; x < IMG_WIDTH; x += 1) {
      // ? is it bad practice that it must be duplicated to prevent double free
      double val = (((double)x / IMG_WIDTH) + ((double)y / IMG_HEIGHT)) / 2.0;
      gsl_vector *color = ray_create_vec3(val, 0.0, 0.0);
      img->pixels[y][x] = color;
    }
  }

  bool success = ray_png_write("image_test.png", img);
  ray_free_img(img);
  if (!success) {
    fprintf(stdout, "failed to write out image\n");
    return 1;
  }

  return 0;
}
