#include "ray/normal.h"

typedef gsl_vector *(*surface_normal_fn)(const RayObject *, const gsl_vector *);

gsl_vector *sphere_surface_normal(const RayObject *sphere,
                                  const gsl_vector *hit_point) {
  gsl_vector *n = gsl_vector_alloc(3);
  gsl_vector_memcpy(n, hit_point);
  gsl_vector_sub(n, sphere->center);
  return n;
}

gsl_vector *plane_surface_normal(const RayObject *plane,
                                 const gsl_vector *hit_point) {
  gsl_vector *n = gsl_vector_alloc(3);
  gsl_vector_memcpy(n, plane->normal);
  gsl_vector_scale(n, -1.0); // negate normal
  return n;
}

gsl_vector *error_surface_normal(const RayObject *plane,
                                 const gsl_vector *hit_point) {
  fprintf(stderr, "invalid object type in surface normal calculation\n");
  exit(1);
}

surface_normal_fn get_surface_normal_fn(enum RAY_OBJECT_TYPE t) {
  return (t == RAY_OBJECT_TYPE_sphere)  ? sphere_surface_normal
         : (t == RAY_OBJECT_TYPE_plane) ? plane_surface_normal
                                        : error_surface_normal;
}

gsl_vector *ray_surface_normal(const RayObject *object,
                               const gsl_vector *hit_point) {
  return get_surface_normal_fn(object->type)(object, hit_point);
}
