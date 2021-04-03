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

#include "ray/render.h"

#include "ray/ray.h"

RayImg *ray_render_scene(const RayScene *scene) {
  RayImg *img = ray_create_img(scene->width, scene->height, 3);
  RayColor black = {0, 0, 0};
  for (int y = 0; y < scene->height; ++y) {
    for (int x = 0; x < scene->width; ++x) {
      RayRay *ray = ray_create_prime_ray(x, y, scene);
      if (ray_sphere_intersects(scene->sphere, ray)) {
        ray_set_pixel(x, y, scene->sphere->color, img);
      } else {
        ray_set_pixel(x, y, black, img);
      }
    }
  }
  return img;
}
