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
#include "ray/render.h"
#include "ray/loader.h"

int main() {
  RayScene scene;
  bool success = ray_scene_from_file("scene.json", &scene);
  if (!success) {
    fprintf(stderr, "failed to load scene.json\n");
    return 1;
  }

  RayImg *img = ray_render_scene(&scene);
  ray_free_scene(&scene);

  ray_png_write("loader_test.png", img);
  ray_free_img(img);

  return 0;
}
