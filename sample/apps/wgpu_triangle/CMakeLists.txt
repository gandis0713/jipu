configure_sample(wgpu_triangle)

find_package(Dawn CONFIG REQUIRED)
target_link_libraries(wgpu_triangle PRIVATE dawn::webgpu_dawn)