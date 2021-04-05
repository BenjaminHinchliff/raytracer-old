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

#ifndef INCLUDED_RAY_INTERSECT_H
#define INCLUDED_RAY_INTERSECT_H

#include "ray/objects.h"
#include "ray/ray.h"

bool ray_intersects(const RayObject *plane, const RayRay *ray,
                    double *distance);

const RayObject *ray_closest_intersection(const RayObject *objects,
                                          int num_objects, const RayRay *ray);

#endif // ifndef INCLUDED_RAY_COLOR_H
