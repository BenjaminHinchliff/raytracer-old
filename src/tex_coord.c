#include "ray/tex_coord.h"

#include <math.h>

#include "gsl/gsl_blas.h"
#include "gsl/gsl_math.h"

#include "ray/vec_utils.h"

bool probablyEqual(double a, double b) {
  const double diff = fabs(a - b);

  a = fabs(a);
  b = fabs(b);
  const double factor = a > b ? a : b;
  const double scaledEpsilon = DBL_EPSILON * factor;

  return diff <= scaledEpsilon;
}

typedef RayTexCoord (*tex_coord_fn)(const RayObject *, gsl_vector *);

RayTexCoord sphere_tex_coord(const RayObject *sphere, gsl_vector *hit_point) {
  gsl_vector *hit_vec = gsl_vector_alloc(3);
  gsl_vector_memcpy(hit_vec, hit_point);
  gsl_vector_sub(hit_vec, sphere->center);
  RayTexCoord coords = {
      .x = (1.0 +
            (atan2(gsl_vector_get(hit_vec, 2), gsl_vector_get(hit_vec, 0)) /
             M_PI)) *
           0.5,
      .y = acos(gsl_vector_get(hit_vec, 1) / sphere->radius) / M_PI,
  };
  gsl_vector_free(hit_vec);
  return coords;
}

RayTexCoord plane_tex_coord(const RayObject *plane, gsl_vector *hit_point) {
  gsl_vector *forward = ray_create_vec3(0.0, 0.0, 1.0);
  gsl_vector *x_axis = gsl_vector_alloc(3);
  gsl_vector_memcpy(x_axis, plane->normal);
  ray_vec3_cross(x_axis, forward);
  gsl_vector_free(forward);
  double length = gsl_blas_dnrm2(x_axis);
  if (probablyEqual(length, 0.0)) {
    gsl_vector *up = ray_create_vec3(0.0, 1.0, 0.0);
    gsl_vector_memcpy(x_axis, plane->normal);
    ray_vec3_cross(x_axis, up);
    gsl_vector_free(up);
  }

  gsl_vector *y_axis = gsl_vector_alloc(3);
  gsl_vector_memcpy(y_axis, plane->normal);
  ray_vec3_cross(y_axis, x_axis);

  gsl_vector *hit_vec = gsl_vector_alloc(3);
  gsl_vector_memcpy(hit_vec, hit_point);
  gsl_vector_sub(hit_vec, plane->point);

  double x = 0.0;
  gsl_blas_ddot(hit_vec, x_axis, &x);
  double y = 0.0;
  gsl_blas_ddot(hit_vec, y_axis, &y);
  RayTexCoord coord = {
      .x = x,
      .y = y,
  };

  gsl_vector_free(x_axis);
  gsl_vector_free(y_axis);

  return coord;
}

RayTexCoord error_tex_coord(const RayObject *sphere, gsl_vector *hit_point) {
  fprintf(stderr, "unknown object type to find the tex coord of\n");
  exit(1);
}

tex_coord_fn get_tex_coord_fn(enum RAY_OBJECT_TYPE t) {
  return (t == RAY_OBJECT_TYPE_sphere)  ? sphere_tex_coord
         : (t == RAY_OBJECT_TYPE_plane) ? plane_tex_coord
                                        : error_tex_coord;
}

RayTexCoord ray_object_tex_coord(const RayObject *object,
                                 gsl_vector *hit_point) {
  return get_tex_coord_fn(object->type)(object, hit_point);
}
