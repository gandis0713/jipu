#pragma once

namespace vkt
{

struct PlatformCreateInfo
{
};

class Platform
{
public:
    Platform(PlatformCreateInfo info) noexcept;
    virtual ~Platform() noexcept;
    virtual void* createVkSurfaceKHR(void* nativeWindow, void* instance) = 0;
};

} // namespace vkt
