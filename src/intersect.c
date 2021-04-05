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

typedef bool (*intersect_fn)(const RayObject *, const RayRay *, double *);

bool ray_sphere_intersects(const RayObject *sphere, const RayRay *ray,
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

bool ray_plane_intersects(const RayObject *plane, const RayRay *ray,
                          double *distance) {
  double denom;
  gsl_blas_ddot(plane->normal, ray->direction, &denom);
  if (denom > 1e-6) {
    gsl_vector *v = gsl_vector_alloc(3);
    gsl_vector_memcpy(v, plane->point);
    gsl_vector_sub(v, ray->origin);
    gsl_blas_ddot(v, plane->normal, distance);
    *distance /= denom;
    gsl_vector_free(v);
    if (*distance >= 0.0) {
      return true;
    }
  }
  return false;
}

bool ray_error_intersect(const RayObject *plane, const RayRay *ray,
                         double *distance) {
  fprintf(stderr, "invalid type in intersection");
  exit(1);
}

intersect_fn get_intersect_fn(enum RAY_OBJECT_TYPE t) {
  return (t == RAY_OBJECT_TYPE_sphere)  ? ray_sphere_intersects
         : (t == RAY_OBJECT_TYPE_plane) ? ray_plane_intersects
                                        : ray_error_intersect;
}

bool ray_intersects(const RayObject *plane, const RayRay *ray,
                    double *distance) {
  get_intersect_fn(plane->type)(plane, ray, distance);
}

const RayObject *ray_closest_intersection(const RayObject *objects,
                                          int num_objects, const RayRay *ray) {
  const RayObject *closest = NULL;
  double closest_distance = 0.0; // not read unless closest not null
  for (int i = 0; i < num_objects; ++i) {
    const RayObject *object = &objects[i];
    double distance = 0.0;
    bool intersects = ray_intersects(object, ray, &distance);
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
