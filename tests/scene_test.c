#include "ray/render.h"
#include "ray/scene.h"
#include "ray/vec_utils.h"

int main() {
  RaySphere objects[] = {
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
                  .g = 104,
                  .b = 104,
              },
      },
  };
  RayScene scene = {
      .width = 800,
      .height = 600,
      .fov = 90.0,
      .num_objects = (sizeof objects) / (sizeof *objects),
      .objects = objects,
  };
  RayImg *img = ray_render_scene(&scene);
  ray_free_scene(&scene);

  ray_png_write("raytraced.png", img);
  ray_free_img(img);

  return 0;
}
