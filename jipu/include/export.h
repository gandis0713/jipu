#pragma once

// clang-format off
#if defined(JIPU_SHARED_LIBRARY)
    #if defined(_WIN32)
        #if defined(JIPU_IMPLEMENTATION)
            #define JIPU_EXPORT __declspec(dllexport)
            #if defined(EXPERIMENTAL)
                #define JIPU_EXPERIMENTAL_EXPORT __declspec(dllexport)            
            #else // defined(EXPERIMENTAL)
                #define JIPU_EXPERIMENTAL_EXPORT
            #endif // defined(EXPERIMENTAL)
        #else // defined(JIPU_IMPLEMENTATION)
            #define JIPU_EXPORT __declspec(dllimport)
            #if defined(EXPERIMENTAL)
                #define JIPU_EXPERIMENTAL_EXPORT __declspec(dllimport)
            #else // defined(EXPERIMENTAL)
                #define JIPU_EXPERIMENTAL_EXPORT
            #endif // defined(EXPERIMENTAL)
        #endif // defined(JIPU_IMPLEMENTATION)
    #else // defined(_WIN32)
        #if defined(JIPU_IMPLEMENTATION)
            #define JIPU_EXPORT __attribute__((visibility("default")))
            #if defined(EXPERIMENTAL)
                #define JIPU_EXPERIMENTAL_EXPORT __attribute__((visibility("default")))
            #else // defined(EXPERIMENTAL)
                #define JIPU_EXPERIMENTAL_EXPORT
            #endif // defined(EXPERIMENTAL)
        #else // defined(JIPU_IMPLEMENTATION)
            #define JIPU_EXPORT
            #define JIPU_EXPERIMENTAL_EXPORT
        #endif // defined(JIPU_IMPLEMENTATION)
    #endif // defined(_WIN32)
#else // defined(JIPU_SHARED_LIBRARY)
    #define JIPU_EXPORT
    #define JIPU_EXPERIMENTAL_EXPORT
#endif // defined(JIPU_SHARED_LIBRARY)
// clang-format on