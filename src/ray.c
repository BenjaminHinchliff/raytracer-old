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

#include "ray/ray.h"

#include "gsl/gsl_blas.h"
#include "gsl/gsl_math.h"
#include <string.h>

#include "ray/vec_utils.h"

RayRay *ray_create_ray(gsl_vector *origin, gsl_vector *direction) {
  RayRay *ray = malloc(sizeof *ray);
  if (ray == NULL) {
    return NULL;
  }
  RayRay tmp_ray = {
      .origin = origin,
      .direction = direction,
  };
  memcpy(ray, &tmp_ray, sizeof *ray);
  return ray;
}

RayRay *ray_create_prime_ray(int x, int y, const RayScene *const scene) {
  double fov_rad = scene->fov * M_PI / 180.0;
  double fov_adjust = tan(fov_rad / 2.0);
  double aspect_ratio = scene->width / scene->height;
  double sensor_x = (((((double)x + 0.5) / (double)scene->width) * 2.0 - 1.0) *
                     aspect_ratio) *
                    fov_adjust;
  double sensor_y =
      (1.0 - (((double)y + 0.5) / (double)scene->height) * 2.0) * fov_adjust;

  gsl_vector *origin = gsl_vector_calloc(3);
  gsl_vector *direction = ray_create_vec3(sensor_x, sensor_y, -1.0);
  double inverse_length = 1.0 / gsl_blas_dnrm2(direction);
  gsl_vector_scale(direction, inverse_length);
}