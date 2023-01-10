#pragma once

#if defined(VKT_SHARED_LIBRARY)
    #if defined(_WIN32)
        #define VKT_EXPORT __declspec(dllexport)
    #else
        #define VKT_EXPORT __attribute__((visibility("default")))
    #endif
#else // defined(VKT_SHARED_LIBRARY)
    #define VKT_EXPORT
#endif // defined(VKT_SHARED_LIBRARY)