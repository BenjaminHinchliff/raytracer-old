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

#include "ray/light.h"

#include <assert.h>
#include <math.h>

#include "gsl/gsl_blas.h"
#include "gsl/gsl_math.h"
#include "ray/vec_utils.h"

typedef gsl_vector *(*direction_from_fn)(const RayLight *, gsl_vector *);

gsl_vector *directional_direction_from(const RayLight *light,
                                       gsl_vector *hit_point) {
  gsl_vector *direction = gsl_vector_alloc(3);
  gsl_vector_memcpy(direction, light->direction);
  gsl_vector_scale(direction, -1.0);
  ray_vec_normalize(direction);
  return direction;
}

gsl_vector *point_direction_from(const RayLight *light, gsl_vector *hit_point) {
  gsl_vector *direction = gsl_vector_alloc(3);
  gsl_vector_memcpy(direction, light->position);
  gsl_vector_sub(direction, hit_point);
  ray_vec_normalize(direction);
  return direction;
}

gsl_vector *error_direction_from(const RayLight *light, gsl_vector *hit_point) {
  fprintf(stderr, "invalid light type in direction_from\n");
  exit(1);
}

direction_from_fn get_direction_from_fn(RAY_LIGHT_TYPE t) {
  return (t == RAY_LIGHT_TYPE_directional) ? directional_direction_from
         : (t == RAY_LIGHT_TYPE_point)     ? point_direction_from
                                           : error_direction_from;
}

gsl_vector *ray_light_direction_from(const RayLight *light,
                                     gsl_vector *hit_point) {
  return get_direction_from_fn(light->type)(light, hit_point);
}

typedef double (*light_intensity_fn)(const RayLight *, gsl_vector *);

double directional_intensity(const RayLight *light, gsl_vector *hit_point) {
  return light->intensity;
}

double point_intensity(const RayLight *light, gsl_vector *hit_point) {
  gsl_vector *direction = gsl_vector_alloc(3);
  gsl_vector_memcpy(direction, light->position);
  gsl_vector_sub(direction, hit_point);
  double r = gsl_blas_dnrm2(
      direction); // ? weirdly none without sqrt (I think?) (ineffecient?)a
  gsl_vector_free(direction);
  double r2 = r * r;
  return light->intensity / (4.0 * M_PI * r2);
}

double error_intensity(const RayLight *light, gsl_vector *hit_point) {
  fprintf(stderr, "unknown light type for intensity\n");
  exit(1);
}

light_intensity_fn get_intensity_fn(RAY_LIGHT_TYPE t) {
  return (t == RAY_LIGHT_TYPE_directional) ? directional_intensity
         : (t == RAY_LIGHT_TYPE_point)     ? point_intensity
                                           : error_intensity;
}

double ray_light_intensity(const RayLight *light, gsl_vector *hit_point) {
  return get_intensity_fn(light->type)(light, hit_point);
}

typedef double (*light_distance_fn)(const RayLight *, gsl_vector *);

double directional_distance(const RayLight *light, gsl_vector *hit_point) {
  return INFINITY;
}

double point_distance(const RayLight *light, gsl_vector *hit_point) {
  gsl_vector *distance = gsl_vector_alloc(3);
  gsl_vector_memcpy(distance, light->position);
  gsl_vector_sub(distance, hit_point);
  double len = gsl_blas_dnrm2(distance);
  gsl_vector_free(distance);
  return len;
}

double error_distance(const RayLight *light, gsl_vector *hit_point) {
  fprintf(stderr, "invalid light type in distance fn\n");
  exit(1);
}

light_distance_fn get_light_distance_fn(RAY_LIGHT_TYPE t) {
  return (t == RAY_LIGHT_TYPE_directional) ? directional_distance
         : (t == RAY_LIGHT_TYPE_point)     ? point_distance
                                           : error_distance;
}

double ray_light_distance(const RayLight *light, gsl_vector *hit_point) {
  return get_light_distance_fn(light->type)(light, hit_point);
}

typedef void (*light_free_fn)(RayLight *light);

void free_directional_light(RayLight *light) {
  gsl_vector_free(light->direction);
}

void free_point_light(RayLight *light) { gsl_vector_free(light->position); }

void free_error_light(RayLight *light) {
  fprintf(stderr, "invalid type of light free\n");
  exit(1);
}

light_free_fn get_light_free_fn(RAY_LIGHT_TYPE t) {
  return (t == RAY_LIGHT_TYPE_directional) ? free_directional_light
         : (t == RAY_LIGHT_TYPE_point)     ? free_point_light
                                           : free_error_light;
}

void ray_free_light(RayLight *light) {
  get_light_free_fn(light->type)(light);
  gsl_vector_free(light->color);
}
