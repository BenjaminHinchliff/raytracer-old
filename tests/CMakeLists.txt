#  a small and simple raytracer
#  Copyright (C) 2021  Benjamin Hinchliff
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
#  USA

add_executable(scene_test "scene_test.c")
target_link_libraries(scene_test PUBLIC ray)
add_test(scene_test scene_test)

add_executable(loader_test "loader_test.c")
target_link_libraries(loader_test PUBLIC ray)
add_test(loader_test loader_test)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/scene.json
  ${CMAKE_CURRENT_BINARY_DIR}/scene.json
  COPYONLY)

add_executable(image_test "image_test.c")
target_link_libraries(image_test PUBLIC ray)
add_test(image_test image_test)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/texture.png
  ${CMAKE_CURRENT_BINARY_DIR}/texture.png
  COPYONLY
)

add_executable(cross_test "cross_test.c")
target_link_libraries(cross_test PUBLIC ray)
add_test(cross_test cross_test)
