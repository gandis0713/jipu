configure_sample(june_triangle)

if(APPLE)
    find_package(unofficial-angle REQUIRED CONFIG)
    target_link_libraries(june_triangle
        PRIVATE
        unofficial::angle::libGLESv2
        unofficial::angle::libEGL
    )
endif()