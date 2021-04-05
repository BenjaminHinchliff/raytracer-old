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

static bool get_obj_color_comp(json_object *obj, const char *key,
                               unsigned char *comp) {
  int val;
  bool success = get_root_int(obj, key, &val);
  if (!success || val < 0 || val > UCHAR_MAX) {
    return false;
  }
  *comp = val;
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
  json_object *objects_obj = json_object_object_get(root, "objects");
  if (objects_obj == NULL ||
      !json_object_is_type(objects_obj, json_type_array)) {
    return false;
  }
  int num_objects = json_object_array_length(objects_obj);
  RaySphere *objects = malloc(num_objects * (sizeof *objects));
  for (int i = 0; i < num_objects; ++i) {
    json_object *object_obj = json_object_array_get_idx(objects_obj, i);
    if (object_obj == NULL) {
      return false;
    }
    json_object *center_obj = json_object_object_get(object_obj, "center");
    if (center_obj == NULL) {
      return false;
    }
    double x;
    success = get_obj_double(center_obj, "x", &x);
    if (!success) {
      return false;
    }
    double y;
    success = get_obj_double(center_obj, "y", &y);
    if (!success) {
      return false;
    }
    double z;
    success = get_obj_double(center_obj, "z", &z);
    if (!success) {
      return false;
    }
    double radius;
    success = get_obj_double(object_obj, "radius", &radius);
    if (!success) {
      return false;
    }
    json_object *color_obj = json_object_object_get(object_obj, "color");
    if (color_obj == NULL) {
      return false;
    }
    unsigned char r;
    success = get_obj_color_comp(color_obj, "r", &r);
    if (!success) {
      return false;
    }
    unsigned char g;
    success = get_obj_color_comp(color_obj, "g", &g);
    if (!success) {
      return false;
    }
    unsigned char b;
    success = get_obj_color_comp(color_obj, "b", &b);
    if (!success) {
      return false;
    }

    RaySphere object = {
        .center = ray_create_vec3(x, y, z),
        .radius = radius,
        .color =
            {
                .r = r,
                .g = g,
                .b = b,
            },
    };
    objects[i] = object;
  }
  *scene = (RayScene){
      .width = width,
      .height = height,
      .fov = fov,
      .num_objects = num_objects,
      .objects = objects,
  };
  json_object_put(root);
  return true;
}
