#include <string.h>

#include "ray/render.h"
#include "ray/scene.h"
#include "ray/vec_utils.h"

#define NUM_OBJECTS 3

int main() {
  // NOTE: using from the stack instead of malloc causes undefined behavior
  // so don't do that
  RaySphere *objects = malloc(NUM_OBJECTS * (sizeof *objects));
  RaySphere stackObjs[NUM_OBJECTS] = {
      {
          .center = ray_create_vec3(0.0, 0.0, -5.0),
          .radius = 1.0,
          .color =
              {
                  .r = 102,
                  .g = 255,
                  .b = 102,
              },
      },
      {
          .center = ray_create_vec3(-3.0, 1.0, -6.0),
          .radius = 2.0,
          .color =
              {
                  .r = 102,
                  .g = 102,
                  .b = 255,
              },
      },
      {
          .center = ray_create_vec3(2.0, 2.0, -4.0),
          .radius = 2.25,
          .color =
              {
                  .r = 255,
                  .g = 102,
                  .b = 102,
              },
      },
  };
  memcpy(objects, stackObjs, NUM_OBJECTS * (sizeof *objects));
  RayScene scene = {
      .width = 800,
      .height = 600,
      .fov = 90.0,
      .num_objects = NUM_OBJECTS,
      .objects = objects,
  };
  RayImg *img = ray_render_scene(&scene);
  ray_free_scene(&scene);

  ray_png_write("scene_test.png", img);
  ray_free_img(img);

  return 0;
}
