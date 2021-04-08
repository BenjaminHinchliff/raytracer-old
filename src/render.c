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

#include <pthread.h>

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

gsl_vector *shade_diffuse_part(RayMaterial material, RayTexCoord tex_coord,
                               gsl_vector *light_color, double light_power,
                               double light_reflected) {
  // calculate the color of the light
  gsl_vector *color = gsl_vector_alloc(3);
  gsl_vector *base_color =
      ray_coloration_color_get(&material.coloration, tex_coord);
  gsl_vector_memcpy(color, base_color);
  gsl_vector_mul(color, light_color);
  gsl_vector_scale(color, light_power);
  gsl_vector_scale(color, light_reflected);
  return color;
}

gsl_vector *shade_diffuse(const RayScene *scene, const RayObject *intersection,
                          gsl_vector *hit_point, gsl_vector *surface_normal) {
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
        shade_diffuse_part(intersection->material, tex_coord, light->color,
                           light_power, light_reflected);
    // add to net color
    gsl_vector_add(color, color_part);
    gsl_vector_free(color_part);
  }

  // clamp between 0.0 & 1.0 to prevent problems with image output
  ray_vec_clamp(color);
  return color;
}

gsl_vector *cast_ray(const RayScene *scene, RayRay ray, int depth);

gsl_vector *get_color(const RayScene *scene, const RayRay ray,
                      const RayObject *intersection, double distance,
                      int depth) {
  gsl_vector *hit_point = get_hit_point(ray, distance);

  gsl_vector *surface_normal = ray_surface_normal(intersection, hit_point);

  gsl_vector *color =
      shade_diffuse(scene, intersection, hit_point, surface_normal);
  if (intersection->material.surface.type == RAY_SURFACE_TYPE_reflective) {
    RayRay reflection_ray = ray_create_reflection(
        surface_normal, ray.direction, hit_point, scene->shadow_bias);
    double reflectivity = intersection->material.surface.reflectivity;
    gsl_vector_scale(color, 1.0 - reflectivity);
    gsl_vector *reflected = cast_ray(scene, reflection_ray, depth + 1);
    gsl_vector_scale(reflected, reflectivity);
    gsl_vector_add(color, reflected);
    ray_ray_free(reflection_ray);
    gsl_vector_free(reflected);
  }

  gsl_vector_free(surface_normal);
  gsl_vector_free(hit_point);

  return color;
}

gsl_vector *cast_ray(const RayScene *scene, RayRay ray, int depth) {
  if (depth > scene->max_recursion_depth) {
    return ray_create_vec3(0.0, 0.0, 0.0);
  }

  double distance = 0.0;
  const RayObject *intersection = ray_closest_intersection(
      scene->objects, scene->num_objects, &ray, &distance);
  if (intersection != NULL) {
    return get_color(scene, ray, intersection, distance, depth);
  } else {
    return ray_create_vec3(0.0, 0.0, 0.0);
  }
}

#define NUM_THREADS 12

typedef struct ThreadImg {
  pthread_mutex_t *lock;
  RayImg *img;
} ThreadImg;

typedef struct RenderSceneRangeArgs {
  const RayScene *scene;
  ThreadImg *thread_img;
  int start;
  int range;
} RenderSceneRangeArgs;

void *ray_render_scene_range(void *voidArgs) {
  RenderSceneRangeArgs *args = voidArgs;
  const RayScene *scene = args->scene;
  ThreadImg *thread_img = args->thread_img;
  RayImg *img = ray_create_img(scene->width, args->range, 3);

  for (int y = 0; y < args->range; y += 1) {
    for (int x = 0; x < scene->width; x += 1) {
      RayRay ray = ray_create_prime_ray(x, args->start + y, scene);
      // get the closest object to the ray
      double distance = 0.0;
      const RayObject *intersection = ray_closest_intersection(
          scene->objects, scene->num_objects, &ray, &distance);
      if (intersection != NULL) {
        gsl_vector *color = get_color(scene, ray, intersection, distance, 0);

        ray_set_pixel(x, y, color, img);
      } else {
        gsl_vector *background = gsl_vector_alloc(3);
        gsl_vector_memcpy(background, scene->background);
        ray_set_pixel(x, y, background, img);
      }
      ray_ray_free(ray);
    }
  }

  pthread_mutex_lock(thread_img->lock);

  for (int y = 0; y < args->range; y += 1) {
    for (int x = 0; x < scene->width; x += 1) {
      thread_img->img->pixels[args->start + y][x] = img->pixels[y][x];
      img->pixels[y][x] = NULL;
    }
  }
  
  pthread_mutex_unlock(thread_img->lock);

  ray_free_img(img);

  return NULL;
}

typedef struct ThreadCtx {
  pthread_t thread;
  RenderSceneRangeArgs *args;
} ThreadCtx;

RayImg *ray_render_scene(const RayScene *scene) {
  pthread_mutex_t lock;

  if (pthread_mutex_init(&lock, NULL) != 0) {
    return NULL;
  }

  RayImg *img = ray_create_img(scene->width, scene->height, 3);
  ThreadImg thread_img = {
      .lock = &lock,
      .img = img,
  };

  const int thread_range = scene->height / NUM_THREADS;
  ThreadCtx threads[NUM_THREADS];
  for (int t = 0; t < NUM_THREADS; t += 1) {
    int adjusted_range = thread_range;
    if (t == (NUM_THREADS - 1)) {
      adjusted_range += (scene->height % NUM_THREADS);
    }
    int start = thread_range * t;
    int range = adjusted_range;
    RenderSceneRangeArgs *args = malloc(sizeof *args);
    *args = (RenderSceneRangeArgs){
        .scene = scene,
        .thread_img = &thread_img,
        .start = start,
        .range = range,
    };
    pthread_t thread;
    if (pthread_create(&thread, NULL, &ray_render_scene_range, args) != 0) {
      return NULL;
    }
    ThreadCtx thread_ctx = {
        .thread = thread,
        .args = args,
    };
    threads[t] = thread_ctx;
  }

  for (int t = 0; t < NUM_THREADS; t += 1) {
    pthread_join(threads[t].thread, NULL);
    free(threads[t].args);
  }

  pthread_mutex_destroy(&lock);
  return img;
}
