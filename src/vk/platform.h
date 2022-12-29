#pragma once

namespace vkt
{

class Platform
{
    virtual void* createSurface(void* nativeWindow, void* instance) = 0;
};

} // namespace vkt
