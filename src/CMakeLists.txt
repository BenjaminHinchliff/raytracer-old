set(HDRS
    "ray/img_utils.h"
    "ray/color.h"
    "ray/objects.h"
    "ray/scene.h"
    "ray/ray.h"
    "ray/vec_utils.h"
    "ray/render.h")

set(HDRS_PREFIX "../include/")

list(TRANSFORM HDRS PREPEND ${HDRS_PREFIX})

set(SRCS
    "img_utils.c"
    "vec_utils.c"
    "objects.c"
    "scene.c"
    "ray.c"
    "render.c")

add_library(ray ${SRCS} ${HDRS})
target_include_directories(ray PUBLIC ${HDRS_PREFIX})

# libm preferred
include(CheckLibraryExists)
check_library_exists(m tan "" LIBM)
if(LIBM)
    target_link_libraries(ray PUBLIC m)
endif()

find_package(PNG REQUIRED)
target_link_libraries(ray PUBLIC PNG::PNG)

find_package(GSL REQUIRED)
target_link_libraries(ray PUBLIC GSL::gsl GSL::gslcblas)
