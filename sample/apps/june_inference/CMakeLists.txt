configure_sample(june_inference)

target_include_directories(june_inference
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/litert_140
)

add_library(litert_140 SHARED IMPORTED)
set_target_properties(litert_140 PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/liblitert_140_jni.so)
target_link_libraries(june_inference
    PRIVATE
    litert_140
)

if(APPLE)
    find_package(unofficial-angle REQUIRED CONFIG)
    target_link_libraries(june_inference
        PRIVATE
        unofficial::angle::libGLESv2
        unofficial::angle::libEGL
    )
endif()
