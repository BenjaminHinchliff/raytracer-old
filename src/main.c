#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "img_utils.h" // RayImg RayColor ray_create_img ray_png_write ray_free_img

#include "gsl/gsl_vector.h"
#include "gsl/gsl_blas.h"

int main(int argc, char **argv) {
  // create image
  const int width = 100;
  const int height = 50;
  const int channels = 3;
  RayImg *img = ray_create_img(width, height, channels);
  for (int y = 0; y < height; y += 1) {
    for (int x = 0; x < width; x += 1) {
      int val = x + y;
      RayColor color = {
          .r = (val > UCHAR_MAX ? UCHAR_MAX : val),
          .g = 0,
          .b = 0,
      };
      ray_set_pixel(x, y, color, img);
    }
  }
  if (!ray_png_write("out.png", img)) {
    fprintf(stderr, "failed to write png to file");
    return 1;
  }
  ray_free_img(img);

  // vector testing
  gsl_vector *vec1 = gsl_vector_calloc(3);
  gsl_vector_set(vec1, 0, 2.0);
  gsl_vector_set(vec1, 1, 7.0);
  gsl_vector_set(vec1, 2, 1.0);

  gsl_vector *vec2 = gsl_vector_calloc(3);
  gsl_vector_set(vec2, 0, 8.0);
  gsl_vector_set(vec2, 1, 2.0);
  gsl_vector_set(vec2, 2, 8.0);

  double dot_result = 0.0;
  gsl_blas_ddot(vec1, vec2, &dot_result);

  printf("result: %f\n", dot_result);

  gsl_vector_free(vec1);
  gsl_vector_free(vec2);

  return 0;
}
