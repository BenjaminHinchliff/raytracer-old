set(HDRS
    "ray/img_utils.h"
    "ray/objects.h"
    "ray/scene.h"
    "ray/ray.h"
    "ray/vec_utils.h"
    "ray/render.h"
    "ray/intersect.h"
    "ray/loader.h"
    "ray/normal.h")

set(HDRS_PREFIX "../include/")

list(TRANSFORM HDRS PREPEND ${HDRS_PREFIX})

set(SRCS
    "img_utils.c"
    "vec_utils.c"
    "objects.c"
    "ray.c"
    "render.c"
    "scene.c"
    "intersect.c"
    "loader.c"
    "normal.c")

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

# find_package for json-c seems to be bugged on arch so use pkg-config first
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(JSON_C IMPORTED_TARGET json-c)
    if(JSON_C_FOUND)
        target_link_libraries(ray PUBLIC PkgConfig::JSON_C)
    endif()
endif()
if(NOT JSON_C_FOUND)
    find_package(json-c CONFIG QUIET)
    if(json-c_FOUND)
        target_link_libraries(ray PUBLIC json-c::json-c)
    else()
        include(FetchContent)
        FetchContent_Declare(
            json-c
            GIT_REPOSITORY https://github.com/json-c/json-c.git
            GIT_TAG json-c-0.15
        )
        FetchContent_MakeAvailable(json-c)
    endif()
endif()
