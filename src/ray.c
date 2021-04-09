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

#include "ray/vec_utils.h"

void ray_ray_free(RayRay ray) {
  gsl_vector_free(ray.origin);
  gsl_vector_free(ray.direction);
}

RayRay ray_create_prime_ray(int x, int y, const RayScene *scene) {
  double fov_rad = scene->fov * M_PI / 180.0;
  double fov_adjust = tan(fov_rad / 2.0);
  double aspect_ratio = (double)scene->width / (double)scene->height;
  double sensor_x = (((((double)x + 0.5) / (double)scene->width) * 2.0 - 1.0) *
                     aspect_ratio) *
                    fov_adjust;
  double sensor_y =
      (1.0 - (((double)y + 0.5) / (double)scene->height) * 2.0) * fov_adjust;

  gsl_vector *origin = gsl_vector_calloc(3);
  gsl_vector *direction = ray_create_vec3(sensor_x, sensor_y, -1.0);
  double inverse_length = 1.0 / gsl_blas_dnrm2(direction);
  gsl_vector_scale(direction, inverse_length);
  RayRay ray = {
      .origin = origin,
      .direction = direction,
  };
  return ray;
}

RayRay ray_create_reflection(gsl_vector *normal, gsl_vector *incident,
                             gsl_vector *intersection, double bias) {
  gsl_vector *origin = gsl_vector_alloc(3);
  gsl_vector_memcpy(origin, normal);
  gsl_vector_scale(origin, bias);
  gsl_vector_add(origin, intersection);

  double norm_dot_incident = 0.0;
  gsl_blas_ddot(incident, normal, &norm_dot_incident);
  gsl_vector *direction = gsl_vector_alloc(3);
  gsl_vector_memcpy(direction, normal);
  gsl_vector_scale(direction, -2.0 * norm_dot_incident);
  gsl_vector_add(direction, incident);

  RayRay ray = {
      .origin = origin,
      .direction = direction,
  };
  return ray;
}

bool ray_create_transmission(RayRay *ray, gsl_vector *normal,
                             gsl_vector *incident, gsl_vector *intersection,
                             double bias, double iof_t) {
  gsl_vector *refrac_n = gsl_vector_alloc(3);
  gsl_vector_memcpy(refrac_n, normal);
  double iof_i = RAY_IOF_I;
  double i_dot_n;
  gsl_blas_ddot(incident, normal, &i_dot_n);
  if (i_dot_n < 0.0) {
    // outside surface
    i_dot_n = -i_dot_n;
  } else {
    // inside so swap iofs and invert normal
    gsl_vector_scale(refrac_n, -1.0);
    iof_i = iof_t;
    iof_t = RAY_IOF_I;
  }

  double iof = iof_i / iof_t;
  double k = 1.0 - ((iof * iof) * (1.0 - (i_dot_n * i_dot_n)));
  if (k < 0.0) {
    gsl_vector_free(refrac_n);
    return false;
  }

  gsl_vector *origin = gsl_vector_alloc(3);
  gsl_vector_memcpy(origin, refrac_n);
  gsl_vector_scale(origin, -bias);
  gsl_vector_add(origin, intersection);

  gsl_vector *direction = gsl_vector_alloc(3);
  gsl_vector_memcpy(direction, refrac_n);
  gsl_vector_scale(direction, i_dot_n);
  gsl_vector_add(direction, incident);
  gsl_vector_scale(direction, iof);

  // repurpose refrac_n for last part of direction
  gsl_vector_scale(refrac_n, sqrt(k));

  gsl_vector_sub(direction, refrac_n);

  *ray = (RayRay){
      .origin = origin,
      .direction = direction,
  };

  gsl_vector_free(refrac_n);
  return true;
}
