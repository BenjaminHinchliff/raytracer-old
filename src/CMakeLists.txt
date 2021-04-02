set(HDRS
    "img_utils.h")

set(SRCS
    "img_utils.c"
    "main.c")

add_executable(ray ${SRCS} ${HDRS})
target_include_directories(ray PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PNG REQUIRED)
target_link_libraries(ray PUBLIC PNG::PNG)

find_package(GSL REQUIRED)
target_link_libraries(ray PUBLIC GSL::gsl GSL::gslcblas)
