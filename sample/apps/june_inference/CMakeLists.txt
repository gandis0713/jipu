configure_sample(june_inference)

find_library(camera2ndk-lib camera2ndk)
find_library(mediandk-lib mediandk)

target_include_directories(june_inference
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/litert_140
    ${CMAKE_CURRENT_SOURCE_DIR}/include/litert_140/external/org_tensorflow

    ${CMAKE_CURRENT_SOURCE_DIR}/include/litert_200

    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(litert_140_jni SHARED IMPORTED)
set_target_properties(litert_140_jni PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libtensorflowlite_jni.so)

add_library(litert_200_jni SHARED IMPORTED)
set_target_properties(litert_200_jni PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/liblitert_jni.so)

add_library(litert_200_GpuAccelerator SHARED IMPORTED)
set_target_properties(litert_200_GpuAccelerator PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libLiteRtGpuAccelerator.so)

add_library(litert_200_RuntimeCApi SHARED IMPORTED)
set_target_properties(litert_200_RuntimeCApi PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libLiteRtRuntimeCApi.so)

target_link_libraries(june_inference
    PRIVATE
    litert_140_jni
    litert_200_jni
    litert_200_RuntimeCApi
    litert_200_GpuAccelerator
    ${camera2ndk-lib}
    ${mediandk-lib}
)
