#pragma once

// clang-format off
#if defined(HPC_SHARED_LIBRARY)
    #if defined(_WIN32)
        #if defined(HPC_IMPLEMENTATION)
            #define HPC_EXPORT __declspec(dllexport)
        #else
            #define HPC_EXPORT __declspec(dllimport)
        #endif
    #else // defined(_WIN32)
        #if defined(HPC_IMPLEMENTATION)
            #define HPC_EXPORT __attribute__((visibility("default")))
        #else
            #define HPC_EXPORT
        #endif
    #endif // defined(_WIN32)
#else      // defined(HPC_SHARED_LIBRARY)
    #define HPC_EXPORT
#endif // defined(HPC_SHARED_LIBRARY)
// clang-format on