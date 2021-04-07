#include <string.h>

#include "ray/render.h"
#include "ray/scene.h"
#include "ray/vec_utils.h"

#define NUM_OBJECTS 4
#define NUM_LIGHTS 2

int main() {
  // NOTE: using from the stack instead of malloc causes undefined behavior
  // so don't do that
  RayObject *objects = malloc(NUM_OBJECTS * (sizeof *objects));
  RayObject stackObjs[NUM_OBJECTS] = {
      {
          .type = RAY_OBJECT_TYPE_sphere,
          .center = ray_create_vec3(0.0, 0.0, -5.0),
          .radius = 1.0,
          .material =
              {
                  .color = ray_create_vec3(0.2, 1.0, 0.2),
                  .albedo = 0.18,
              },
      },
      {
          .type = RAY_OBJECT_TYPE_sphere,
          .center = ray_create_vec3(-3.0, 1.0, -6.0),
          .radius = 2.0,
          .material =
              {
                  .color = ray_create_vec3(0.2, 0.2, 1.0),
                  .albedo = 0.28,
              },
      },
      {
          .type = RAY_OBJECT_TYPE_sphere,
          .center = ray_create_vec3(2.0, 2.0, -4.0),
          .radius = 2.25,
          .material =
              {
                  .color = ray_create_vec3(1.0, 0.2, 0.2),
                  .albedo = 0.08,
              },
      },
      {
          .type = RAY_OBJECT_TYPE_plane,
          .point = ray_create_vec3(0.0, -2.0, 0.0),
          .normal = ray_create_vec3(0.0, -1.0, 0.0),
          .material =
              {
                  .color = ray_create_vec3(0.4, 0.4, 0.4),
                  .albedo = 0.18,
              },
      },
  };
  memcpy(objects, stackObjs, NUM_OBJECTS * (sizeof *objects));

  RayLight *lights = malloc(NUM_LIGHTS * (sizeof *lights));
  RayLight stackLights[NUM_LIGHTS] = {
      {
          .type = RAY_LIGHT_TYPE_directional,
          .direction = ray_create_vec3(-0.5, -1.0, -1.0),
          .color = ray_create_vec3(0.3, 0.8, 0.3),
          .intensity = 10.0,
      },
      {
          .type = RAY_LIGHT_TYPE_directional,
          .direction = ray_create_vec3(0.5, -1.0, -1.0),
          .color = ray_create_vec3(0.8, 0.3, 0.3),
          .intensity = 10.0,
      },
  };
  memcpy(lights, stackLights, NUM_LIGHTS * (sizeof *lights));

  RayScene scene = {
      .width = 800,
      .height = 600,
      .fov = 90.0,
      .background = ray_create_vec3(0.73, 0.92, 1.0),
      .num_objects = NUM_OBJECTS,
      .objects = objects,
      .num_lights = NUM_LIGHTS,
      .lights = lights,
  };
  RayImg *img = ray_render_scene(&scene);
  ray_free_scene(&scene);

  ray_png_write("scene_test.png", img);
  ray_free_img(img);

  return 0;
}
