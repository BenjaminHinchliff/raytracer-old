#include "ray/render.h"
#include "ray/scene.h"
#include "ray/vec_utils.h"

int main() {
  RayScene scene = {
      .width = 800,
      .height = 600,
      .fov = 90.0,
      .sphere =
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
  };
  RayImg *img = ray_render_scene(scene);
  ray_free_scene(scene);

  ray_png_write("raytraced.png", img);
  ray_free_img(img);

  return 0;
}
