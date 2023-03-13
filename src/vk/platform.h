#pragma once

namespace vkt
{

class Platform
{
public:
    virtual void* createVkSurfaceKHR(void* nativeWindow, void* instance) = 0;
};

} // namespace vkt
