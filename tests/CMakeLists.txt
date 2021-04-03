add_executable(scene_test "scene_test.c")
target_link_libraries(scene_test PUBLIC ray)
add_test(scene_test scene_test)
