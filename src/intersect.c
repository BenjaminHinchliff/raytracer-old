#include "ray/intersect.h"

bool ray_sphere_intersects(const RaySphere *sphere, const RayRay *ray) {
  gsl_vector *l = gsl_vector_alloc(3);
  gsl_vector_memcpy(l, sphere->center);
  gsl_vector_sub(l, ray->origin);
  double adj2;
  gsl_blas_ddot(l, ray->direction, &adj2);
  double d2;
  gsl_blas_ddot(l, l, &d2);
  d2 -= (adj2 * adj2);
  gsl_vector_free(l);
  return d2 < (sphere->radius * sphere->radius);
}
