configure_sample(wgpu_triangle)

find_library(dawn NAMES webgpu_dawn HINTS ${VCPKG_INSTALLED_DIR})
target_link_libraries(wgpu_triangle
    PRIVATE
    ${dawn}
)