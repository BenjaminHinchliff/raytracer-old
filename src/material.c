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

#include "ray/material.h"

#include <assert.h>
#include <math.h>

#include "gsl/gsl_blas.h"
#include "gsl/gsl_math.h"

#include "ray/vec_utils.h"

typedef gsl_vector *(*color_get_fn)(RayColoration *, RayTexCoord);

gsl_vector *color_color_get(RayColoration *coloration, RayTexCoord tex_coord) {
  return coloration->color;
}

int wrap(double val, int bound) {
  double fp_coord = val * bound;
  double wrapped_coord = (int)fp_coord % bound;
  if (wrapped_coord < 0) {
    return wrapped_coord + bound;
  } else {
    return wrapped_coord;
  }
}

gsl_vector *texture_color_get(RayColoration *coloration,
                              RayTexCoord tex_coord) {
  int tex_x = wrap(tex_coord.x, coloration->texture->width);
  assert(tex_x >= 0 && "tex x must be at least 0");
  assert(tex_x < coloration->texture->width && "tex x must be less than width");
  int tex_y = wrap(tex_coord.y, coloration->texture->height);
  assert(tex_y >= 0 && "tex y must be at least 0");
  assert(tex_y < coloration->texture->height &&
         "tex y must be less than height");

  return coloration->texture->pixels[tex_y][tex_x];
}

gsl_vector *error_color_get(RayColoration *coloration, RayTexCoord tex_coord) {
  fprintf(stderr, "invalid coloration type in color get\n");
  exit(1);
}

color_get_fn get_color_get_fn(RAY_COLORATION_TYPE t) {
  return (t == RAY_COLORATION_TYPE_color)     ? color_color_get
         : (t == RAY_COLORATION_TYPE_texture) ? texture_color_get
                                              : error_color_get;
}

gsl_vector *ray_coloration_color_get(RayColoration *coloration,
                                     RayTexCoord tex_coord) {
  return get_color_get_fn(coloration->type)(coloration, tex_coord);
}

typedef void (*free_coloration_fn)(RayColoration *);

void free_color_coloration(RayColoration *coloration) {
  gsl_vector_free(coloration->color);
}

void free_texture_coloration(RayColoration *coloration) {
  ray_free_img(coloration->texture);
}

void free_error_coloration(RayColoration *coloration) {
  fprintf(stderr, "invalid coloration type in coloration free\n");
  exit(1);
}

free_coloration_fn get_free_coloration_fn(RAY_COLORATION_TYPE t) {
  return (t == RAY_COLORATION_TYPE_color)     ? free_color_coloration
         : (t == RAY_COLORATION_TYPE_texture) ? free_texture_coloration
                                              : free_error_coloration;
}

void ray_free_coloration(RayColoration *coloration) {
  get_free_coloration_fn(coloration->type)(coloration);
}

void ray_free_material(RayMaterial *material) {
  ray_free_coloration(&material->coloration);
}
