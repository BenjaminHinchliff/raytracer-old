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

#ifndef INCLUDED_RAY_VEC_UTILS_H
#define INCLUDED_RAY_VEC_UTILS_H

#include "gsl/gsl_vector.h"

gsl_vector *ray_create_vec3(double x, double y, double z);

void ray_vec_normalize(gsl_vector *vec);

void ray_vec_clamp(gsl_vector *vec);

void ray_vec3_cross(gsl_vector *u, gsl_vector *v);

#endif // ifndef INCLUDED_RAY_VEC_UTILS_H
