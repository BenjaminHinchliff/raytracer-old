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

#include "ray/loader.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>

#include "json-c/json.h"

#include "ray/material.h"
#include "ray/vec_utils.h"

static bool get_root_int(json_object *root, const char *key, int *val) {
  json_object *int_obj = json_object_object_get(root, key);
  if (int_obj == NULL) {
    return false;
  }
  errno = 0;
  *val = json_object_get_int(int_obj);
  if (*val == 0 && errno == EINVAL) {
    return false;
  }
  return true;
}

static bool get_obj_double(json_object *root, const char *key, double *val) {
  json_object *int_obj = json_object_object_get(root, key);
  if (int_obj == NULL) {
    return false;
  }
  errno = 0;
  *val = json_object_get_double(int_obj);
  if (*val == 0.0 && errno == EINVAL) {
    return false;
  }
  return true;
}

static gsl_vector *get_obj_vec3(json_object *obj, const char *key) {
  json_object *vec3_obj = json_object_object_get(obj, key);
  if (vec3_obj == NULL) {
    return NULL;
  }
  double x;
  bool success = get_obj_double(vec3_obj, "x", &x);
  if (!success) {
    return NULL;
  }
  double y;
  success = get_obj_double(vec3_obj, "y", &y);
  if (!success) {
    return NULL;
  }
  double z;
  success = get_obj_double(vec3_obj, "z", &z);
  if (!success) {
    return NULL;
  }
  return ray_create_vec3(x, y, z);
}

static gsl_vector *get_obj_rgb(json_object *obj, const char *key) {
  json_object *vec3_obj = json_object_object_get(obj, key);
  if (vec3_obj == NULL) {
    return NULL;
  }
  double r;
  bool success = get_obj_double(vec3_obj, "r", &r);
  if (!success) {
    return NULL;
  }
  double g;
  success = get_obj_double(vec3_obj, "g", &g);
  if (!success) {
    return NULL;
  }
  double b;
  success = get_obj_double(vec3_obj, "b", &b);
  if (!success) {
    return NULL;
  }
  return ray_create_vec3(r, g, b);
}

static bool get_obj_material(json_object *obj, RayMaterial *material) {
  json_object *material_obj = json_object_object_get(obj, "material");
  if (material_obj == NULL) {
    return false;
  }

  gsl_vector *color = get_obj_rgb(material_obj, "color");
  if (color == NULL) {
    return false;
  }

  double albedo;
  bool success = get_obj_double(material_obj, "albedo", &albedo);
  if (!success) {
    return false;
  }

  *material = (RayMaterial){
      .color = color,
      .albedo = albedo,
  };

  return true;
}

static bool get_obj_sphere(json_object *sphere_obj, RayObject *sphere) {
  gsl_vector *center = get_obj_vec3(sphere_obj, "center");
  if (center == NULL) {
    return false;
  }
  double radius;
  bool success = get_obj_double(sphere_obj, "radius", &radius);
  if (!success) {
    return false;
  }

  RayMaterial material;
  success = get_obj_material(sphere_obj, &material);
  if (!success) {
    return false;
  }

  *sphere = (RayObject){
      .type = RAY_OBJECT_TYPE_sphere,
      .center = center,
      .radius = radius,
      .material = material,
  };
  return true;
}

static bool get_obj_plane(json_object *plane_obj, RayObject *plane) {
  gsl_vector *point = get_obj_vec3(plane_obj, "point");
  if (point == NULL) {
    return false;
  }

  gsl_vector *normal = get_obj_vec3(plane_obj, "normal");
  if (normal == NULL) {
    return false;
  }

  RayMaterial material;
  bool success = get_obj_material(plane_obj, &material);
  if (!success) {
    return false;
  }

  *plane = (RayObject){
      .type = RAY_OBJECT_TYPE_plane,
      .point = point,
      .normal = normal,
      .material = material,
  };

  return true;
}

static bool get_scene_object(json_object *source, RayObject *object) {
  json_object *sphere_obj = json_object_object_get(source, "sphere");
  if (sphere_obj != NULL) {
    return get_obj_sphere(sphere_obj, object);
  }
  json_object *plane_obj = json_object_object_get(source, "plane");
  if (plane_obj != NULL) {
    return get_obj_plane(plane_obj, object);
  }
  return false;
}

static RayObject *get_scene_objects(json_object *source, int *num_objects) {
  json_object *objects_obj = json_object_object_get(source, "objects");
  if (objects_obj == NULL ||
      !json_object_is_type(objects_obj, json_type_array)) {
    return NULL;
  }
  *num_objects = json_object_array_length(objects_obj);
  RayObject *objects = malloc(*num_objects * (sizeof *objects));
  for (int i = 0; i < *num_objects; ++i) {
    json_object *object_obj = json_object_array_get_idx(objects_obj, i);
    if (object_obj == NULL) {
      return NULL;
    }

    RayObject object;
    bool success = get_scene_object(object_obj, &object);
    if (!success) {
      return NULL;
    }

    objects[i] = object;
  }
  return objects;
}

static bool get_scene_light(json_object *source, RayLight *light) {
  json_object *light_obj = json_object_object_get(source, "light");
  if (light_obj == NULL) {
    return false;
  }

  gsl_vector *direction = get_obj_vec3(light_obj, "direction");
  if (direction == NULL) {
    return false;
  }

  gsl_vector *color = get_obj_rgb(light_obj, "color");
  if (color == NULL) {
    return false;
  }

  double intensity;
  bool success = get_obj_double(light_obj, "intensity", &intensity);
  if (!success) {
    return false;
  }

  *light = (RayLight){
      .direction = direction,
      .color = color,
      .intensity = intensity,
  };

  return true;
}

bool ray_scene_from_file(const char *path, RayScene *scene) {
  json_object *root = json_object_from_file(path);
  if (root == NULL) {
    return false;
  }
  int width;
  bool success = get_root_int(root, "width", &width);
  if (!success) {
    return false;
  }
  int height;
  success = get_root_int(root, "height", &height);
  if (!success) {
    return false;
  }
  double fov;
  success = get_obj_double(root, "fov", &fov);
  if (!success) {
    return false;
  }

  int num_objects;
  RayObject *objects = get_scene_objects(root, &num_objects);
  if (objects == NULL) {
    return false;
  }

  RayLight light;
  success = get_scene_light(root, &light);
  if (!success) {
    return false;
  }

  *scene = (RayScene){
      .width = width,
      .height = height,
      .fov = fov,
      .num_objects = num_objects,
      .objects = objects,
      .light = light,
  };
  json_object_put(root);
  return true;
}
