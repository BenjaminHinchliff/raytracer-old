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

#include "ray/objects.h"

#include <assert.h>

#include "gsl/gsl_blas.h"

typedef void (*obj_free_fn)(RayObject);

void free_sphere(RayObject sphere) { gsl_vector_free(sphere.center); }

void free_plane(RayObject plane) {
  gsl_vector_free(plane.point);
  gsl_vector_free(plane.normal);
}

void free_error(RayObject err) {
  fprintf(stderr, "invalid object free");
  exit(1);
}

obj_free_fn get_obj_free_fn(enum RAY_OBJECT_TYPE t) {
  return (t == RAY_OBJECT_TYPE_sphere)  ? free_sphere
         : (t == RAY_OBJECT_TYPE_plane) ? free_plane
                                        : free_error;
}

void ray_free_object(RayObject sphere) { get_obj_free_fn(sphere.type)(sphere); }
