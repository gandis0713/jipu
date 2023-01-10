#pragma once

#include "vk/visibility.h"

namespace vkt
{

class VKT_EXPORT Platform
{
    virtual void* createSurface(void* nativeWindow, void* instance) = 0;
};

} // namespace vkt
