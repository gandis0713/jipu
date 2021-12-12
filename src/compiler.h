#pragma once

#if defined(VKS_SHARED_LIBRARY)
#    if defined(_WIN32)
#        if defined(VKS_IMPLEMENTATION)
#            define VKS_EXPORT __declspec(dllexport)
#        else
#            define VKS_EXPORT __declspec(dllimport)
#        endif
#    else  // defined(_WIN32)
#        if defined(VKS_IMPLEMENTATION)
#            define VKS_EXPORT __attribute__((visibility("default")))
#        else
#            define VKS_EXPORT
#        endif
#    endif  // defined(_WIN32)
#else       // defined(VKS_SHARED_LIBRARY)
#    define VKS_EXPORT
#endif  // defined(VKS_SHARED_LIBRARY)