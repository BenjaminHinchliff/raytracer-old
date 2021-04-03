#include "ray/scene.h"
#include "ray/vec_utils.h"

int main() {
  gsl_vector *center = ray_create_vec3(0.0, 0.0, -5.0);
  RaySphere *sphere = ray_create_sphere(center, 1.0,
                                        (RayColor){
                                            .r = 102,
                                            .g = 255,
                                            .b = 102,
                                        });
  RayScene *scene = ray_create_scene(800, 600, 90.0, sphere);

  return 0;
}
