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

#include "ray/intersect.h"
#include "ray/ray.h"
#include "ray/vec_utils.h"

RayImg *ray_render_scene(const RayScene *scene) {
  RayImg *img = ray_create_img(scene->width, scene->height, 3);
  for (int y = 0; y < scene->height; ++y) {
    for (int x = 0; x < scene->width; ++x) {
      RayRay ray = ray_create_prime_ray(x, y, scene);
      double distance = 0.0;
      const RayObject *intersection = ray_closest_intersection(
          scene->objects, scene->num_objects, &ray, &distance);
      if (intersection != NULL) {
        gsl_vector *hit_point = gsl_vector_alloc(3);
        gsl_vector_memcpy(hit_point, ray.direction);
        gsl_vector_scale(hit_point, distance);
        gsl_vector_add(hit_point, ray.origin);

        gsl_vector *color = gsl_vector_alloc(3);
        gsl_vector_memcpy(color, intersection->material.color);
        ray_set_pixel(x, y, color, img);
      } else {
        gsl_vector *black = ray_create_vec3(0.0, 0.0, 0.0);
        ray_set_pixel(x, y, black, img);
      }
      ray_ray_free(ray);
    }
  }
  return img;
}
