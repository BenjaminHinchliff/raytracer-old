#include "ray/img_utils.h"
#include "ray/vec_utils.h"

#define IMG_WIDTH 50
#define IMG_HEIGHT 20
#define IMG_CHANNELS 3

int main() {
  RayImg *img = ray_create_img(IMG_WIDTH, IMG_HEIGHT, IMG_CHANNELS);
  for (int y = 0; y < IMG_HEIGHT; y += 1) {
    for (int x = 0; x < IMG_WIDTH; x += 1) {
      // ? is it bad practice that it must be duplicated to prevent double free
      double val = (((double)x / IMG_WIDTH) + ((double)y / IMG_HEIGHT)) / 2.0;
      gsl_vector *color = ray_create_vec3(val, 0.0, 0.0);
      img->pixels[y][x] = color;
    }
  }

  bool success = ray_png_write("image_test.png", img);
  ray_free_img(img);
  if (!success) {
    fprintf(stdout, "failed to write out image\n");
    return 1;
  }

  return 0;
}
