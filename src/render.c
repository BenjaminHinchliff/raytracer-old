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

#include "ray/render.h"

#include "gsl/gsl_blas.h"
#include "gsl/gsl_math.h"

#include "ray/intersect.h"
#include "ray/normal.h"
#include "ray/ray.h"
#include "ray/tex_coord.h"
#include "ray/vec_utils.h"

static gsl_vector *get_hit_point(const RayRay ray, double hit_distance) {
  gsl_vector *hit_point = gsl_vector_alloc(3);
  gsl_vector_memcpy(hit_point, ray.direction);
  gsl_vector_scale(hit_point, hit_distance);
  gsl_vector_add(hit_point, ray.origin);
  return hit_point;
}

static bool is_in_light(gsl_vector *surface_normal, gsl_vector *hit_point,
                        gsl_vector *dir_to_light, double light_distance,
                        const RayScene *scene) {
  // find the shadow origin by adding a small fudge factor to the hit
  // point (to prevent shadow acne)
  gsl_vector *shadow_origin = gsl_vector_alloc(3);
  gsl_vector_memcpy(shadow_origin, surface_normal);
  gsl_vector_scale(shadow_origin, scene->shadow_bias);
  gsl_vector_add(shadow_origin, hit_point);
  RayRay shadow_ray = {
      .origin = shadow_origin,
      .direction = dir_to_light,
  };

  // ray from hit point to light for shadows on other objects
  double intersect_distance = 0.0;
  const RayObject *in_light_intersect = ray_closest_intersection(
      scene->objects, scene->num_objects, &shadow_ray, &intersect_distance);
  // free allocated vectors
  gsl_vector_free(shadow_origin);
  return in_light_intersect == NULL || intersect_distance > light_distance;
}

// mathy stuff to calculate the light power
// (cos of the angle between the surface normal and the vector to
// light) (plus some stuff to handle clamp negative values and the
// intensity)
static double get_light_power(gsl_vector *surface_normal,
                              gsl_vector *dir_to_light,
                              double light_intensity) {
  double light_power = 0.0;
  gsl_blas_ddot(surface_normal, dir_to_light, &light_power);
  light_power = light_power < 0.0 ? 0.0 : light_power;
  light_power *= light_intensity;
  return light_power;
}

gsl_vector *get_light_color_part(RayMaterial material, RayTexCoord tex_coord,
                                 gsl_vector *light_color, double light_power,
                                 double light_reflected) {
  // calculate the color of the light
  gsl_vector *color = gsl_vector_alloc(3);
  gsl_vector *base_color = ray_coloration_color_get(&material.coloration, tex_coord);
  gsl_vector_memcpy(color, base_color);
  gsl_vector_mul(color, light_color);
  gsl_vector_scale(color, light_power);
  gsl_vector_scale(color, light_reflected);
  return color;
}

RayImg *ray_render_scene(const RayScene *scene) {
  RayImg *img = ray_create_img(scene->width, scene->height, 3);
  for (int y = 0; y < scene->height; ++y) {
    for (int x = 0; x < scene->width; ++x) {
      RayRay ray = ray_create_prime_ray(x, y, scene);
      // get the closest object to the ray
      double distance = 0.0;
      const RayObject *intersection = ray_closest_intersection(
          scene->objects, scene->num_objects, &ray, &distance);
      if (intersection != NULL) {
        gsl_vector *hit_point = get_hit_point(ray, distance);

        gsl_vector *surface_normal =
            ray_surface_normal(intersection, hit_point);

        gsl_vector *color = gsl_vector_calloc(3);
        for (int l = 0; l < scene->num_lights; ++l) {
          const RayLight *light = &scene->lights[l];

          // get the normal to any light
          gsl_vector *dir_to_light = ray_light_direction_from(light, hit_point);

          double light_distance = ray_light_distance(light, hit_point);

          bool in_light = is_in_light(surface_normal, hit_point, dir_to_light,
                                      light_distance, scene);

          double light_intensity =
              in_light ? ray_light_intensity(light, hit_point) : 0.0;

          double light_power =
              get_light_power(surface_normal, dir_to_light, light_intensity);

          // light math is done so so is this
          gsl_vector_free(dir_to_light);

          // calculate amount of reflected light based of albedo
          double light_reflected = intersection->material.albedo / M_PI;

          RayTexCoord tex_coord = ray_object_tex_coord(intersection, hit_point);

          gsl_vector *color_part =
              get_light_color_part(intersection->material, tex_coord, light->color,
                                   light_power, light_reflected);
          // add to net color
          gsl_vector_add(color, color_part);
          gsl_vector_free(color_part);
        }

        gsl_vector_free(surface_normal);
        gsl_vector_free(hit_point);

        // clamp between 0.0 & 1.0 to prevent problems with image output
        ray_vec_clamp(color);
        ray_set_pixel(x, y, color, img);
      } else {
        gsl_vector *background = gsl_vector_alloc(3);
        gsl_vector_memcpy(background, scene->background);
        ray_set_pixel(x, y, background, img);
      }
      ray_ray_free(ray);
    }
  }
  return img;
}
