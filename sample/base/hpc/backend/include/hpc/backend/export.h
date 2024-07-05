#pragma once

// clang-format off
#if defined(HPC_DEVICE_SHARED_LIBRARY)
    #if defined(_WIN32)
        #if defined(HPC_DEVICE_IMPLEMENTATION)
            #define HPC_BACKEND_VISIBILITY __declspec(dllexport)
        #else
            #define HPC_BACKEND_VISIBILITY __declspec(dllimport)
        #endif
    #else // defined(_WIN32)
        #if defined(HPC_DEVICE_IMPLEMENTATION)
            #define HPC_BACKEND_VISIBILITY __attribute__((visibility("default")))
        #else
            #define HPC_BACKEND_VISIBILITY
        #endif
    #endif // defined(_WIN32)
#else      // defined(HPC_DEVICE_SHARED_LIBRARY)
    #define HPC_BACKEND_VISIBILITY
#endif // defined(HPC_DEVICE_SHARED_LIBRARY)
// clang-format on