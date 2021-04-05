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
