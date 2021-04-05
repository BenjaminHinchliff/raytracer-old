#include "ray/img_utils.h"
#include "ray/render.h"
#include "ray/loader.h"

int main() {
  RayScene scene;
  bool success = ray_scene_from_file("scene.json", &scene);
  if (!success) {
    fprintf(stderr, "failed to load scene.json\n");
    return 1;
  }

  RayImg *img = ray_render_scene(&scene);
  ray_free_scene(&scene);

  ray_png_write("loader_test.png", img);
  ray_free_img(img);

  return 0;
}
