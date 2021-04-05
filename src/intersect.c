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

#include "ray/intersect.h"

#include <math.h>

#include "gsl/gsl_blas.h"

bool ray_sphere_intersects(const RaySphere *sphere, const RayRay *ray,
                           double *distance) {
  gsl_vector *l = gsl_vector_alloc(3);
  gsl_vector_memcpy(l, sphere->center);
  gsl_vector_sub(l, ray->origin);
  double adj;
  gsl_blas_ddot(l, ray->direction, &adj);
  double d2;
  gsl_blas_ddot(l, l, &d2);
  d2 -= (adj * adj);
  gsl_vector_free(l);
  double radius2 = sphere->radius * sphere->radius;
  if (d2 > radius2) {
    return false;
  }

  double thc = sqrt(radius2 - d2);
  double i0 = adj - thc;
  double i1 = adj + thc;

  if (i0 < 0.0 && i1 < 0.0) {
    return false;
  }

  *distance = (i0 < i1 ? i0 : i1);
  return true;
}

const RaySphere *ray_closest_intersection(const RaySphere *objects,
                                          int num_objects, const RayRay *ray) {
  const RaySphere *closest = NULL;
  double closest_distance = 0.0; // not read unless closest not null 
  for (int i = 0; i < num_objects; ++i) {
    const RaySphere *object = &objects[i];
    double distance;
    bool intersects = ray_sphere_intersects(object, ray, &distance);
    if (intersects) {
      if (closest == NULL) {
        closest = object;
        closest_distance = distance;
      } else if (distance < closest_distance) {
        closest = object;
        closest_distance = distance;
      }
    }
  }
  return closest;
}
