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

#ifndef INCLUDED_RAY_RAY_H
#define INCLUDED_RAY_RAY_H

#include "ray.h"
#include "scene.h"

#include "gsl/gsl_vector.h"

typedef struct RayRay {
  gsl_vector *const origin;
  gsl_vector *const direction;
} RayRay;

void ray_ray_free(RayRay ray);

RayRay ray_create_prime_ray(int x, int y, RayScene scene);
bool ray_sphere_intersects(RaySphere sphere, RayRay ray);

#endif // ifndef INCLUDED_RAY_RAY_H
