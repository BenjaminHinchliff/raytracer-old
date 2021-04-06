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
#include "ray/vec_utils.h"

#define SHADOW_BIAS 1e-12

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
        // calculate the point at which the object hit
        gsl_vector *hit_point = gsl_vector_alloc(3);
        gsl_vector_memcpy(hit_point, ray.direction);
        gsl_vector_scale(hit_point, distance);
        gsl_vector_add(hit_point, ray.origin);

        // get the surface normal for lighting
        gsl_vector *surface_normal =
            ray_surface_normal(intersection, hit_point);

        // get the normal to a light by inverting the light direction
        gsl_vector *dir_to_light = gsl_vector_alloc(3);
        gsl_vector_memcpy(dir_to_light, scene->light.direction);
        gsl_vector_scale(dir_to_light, -1.0);
        // normalize direction to light to prevent weird issues
        double dir_len = gsl_blas_dnrm2(dir_to_light);
        gsl_vector_scale(dir_to_light, 1.0 / dir_len);

        // find the shadow origin by adding a small fudge factor to the hit
        // point
        gsl_vector *shadow_origin = gsl_vector_alloc(3);
        gsl_vector_memcpy(shadow_origin, surface_normal);
        gsl_vector_scale(shadow_origin, SHADOW_BIAS);
        gsl_vector_add(shadow_origin, hit_point);
        RayRay shadow_ray = {
            .origin = shadow_origin,
            .direction = dir_to_light,
        };
        gsl_vector_free(hit_point);

        // ray from hit point to light for shadows on other objects
        const RayObject *in_light_intersect = ray_closest_intersection(
            scene->objects, scene->num_objects, &shadow_ray, NULL);
        bool in_light = in_light_intersect == NULL;

        double light_intensity = in_light ? scene->light.intensity : 0.0;
        gsl_vector_free(shadow_origin);

        // mathy stuff to calculate the light power
        // (cos of the angle between the surface normal and the vector to light)
        // (plus some stuff to handle clamp negative values and the intensity)
        double light_power = 0.0;
        gsl_blas_ddot(surface_normal, dir_to_light, &light_power);
        gsl_vector_free(dir_to_light);
        gsl_vector_free(surface_normal);
        light_power = light_power < 0.0 ? 0.0 : light_power;
        light_power *= light_intensity;

        // calculate amount of reflected light based of albedo
        double light_reflected = intersection->material.albedo / M_PI;

        // calculate the color of the light
        gsl_vector *color = gsl_vector_alloc(3);
        gsl_vector_memcpy(color, intersection->material.color);
        gsl_vector_mul(color, scene->light.color);
        gsl_vector_scale(color, light_power);
        gsl_vector_scale(color, light_reflected);
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
