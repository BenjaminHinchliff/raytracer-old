#include "ray/vec_utils.h"

#include <assert.h>

int main() {
  gsl_vector *u = ray_create_vec3(2.0, 3.0, 4.0);
  gsl_vector *v = ray_create_vec3(5.0, 6.0, 7.0);
  ray_vec3_cross(u, v);
  gsl_vector *res = ray_create_vec3(-3.0, 6.0, -3.0);
  assert(gsl_vector_equal(u, res) && "cross product must work correctly");
  gsl_vector_free(res);
  gsl_vector_free(v);
  gsl_vector_free(u);
  return 0;
}
