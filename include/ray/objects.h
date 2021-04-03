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

#ifndef INCLUDED_RAY_OBJECTS_H
#define INCLUDED_RAY_OBJECTS_H

#include <stdbool.h>

#include "gsl/gsl_vector.h"

#include "color.h"

typedef struct RaySphere {
  gsl_vector *center;
  double radius;
  RayColor color;
} RaySphere;

RaySphere *ray_create_sphere(gsl_vector *center, double radius, RayColor color);

#endif // ifndef INCLUDED_RAY_OBJECTS_H
