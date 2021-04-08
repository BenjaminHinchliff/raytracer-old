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

#ifndef INCLUDED_RAY_MATERIAL_H
#define INCLUDED_RAY_MATERIAL_H

#include "img_utils.h"
#include "tex_coord.h"

#include "gsl/gsl_vector.h"

typedef enum RAY_COLORATION_TYPE {
  RAY_COLORATION_TYPE_color,
  RAY_COLORATION_TYPE_texture,
} RAY_COLORATION_TYPE;

typedef struct RayColoration {
  RAY_COLORATION_TYPE type;
  union {
    struct { // type = color
      gsl_vector *color;
    };
    struct { // type = texture
      RayImg *texture;
    };
  };
} RayColoration;

gsl_vector *ray_coloration_color_get(RayColoration *, RayTexCoord hit_point);

void ray_free_coloration(RayColoration *coloration);

typedef struct RayMaterial {
  RayColoration coloration;
  double albedo;
} RayMaterial;

void ray_free_material(RayMaterial *material);

#endif // ifndef INCLUDED_RAY_MATERIAL_H
