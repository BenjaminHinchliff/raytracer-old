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
  RayImg *img = ray_read_img("texture.png");

  bool success = ray_png_write("image_test.png", img);
  ray_free_img(img);
  if (!success) {
    fprintf(stdout, "failed to write out image\n");
    return 1;
  }

  return 0;
}
