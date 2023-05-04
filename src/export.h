#pragma once

#if defined(VKT_SHARED_LIBRARY)
    #if defined(_WIN32)
        #if defined(VKT_IMPLEMENTATION)
            #define VKT_EXPORT __declspec(dllexport)
        #else
            #define VKT_EXPORT __declspec(dllimport)
        #endif
    #else // defined(_WIN32)
        #if defined(VKT_IMPLEMENTATION)
            #define VKT_EXPORT __attribute__((visibility("default")))
        #else
            #define VKT_EXPORT
        #endif
    #endif // defined(_WIN32)
#else      // defined(VKT_SHARED_LIBRARY)
    #define VKT_EXPORT
#endif // defined(VKT_SHARED_LIBRARY)