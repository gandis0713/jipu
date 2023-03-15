#pragma once

#include "vk/driver.h"
#include "vk/surface.h"

#include <memory>

namespace vkt
{

struct PlatformCreateInfo
{
    Adapter adapter;
};

class Platform
{
public:
    Platform(PlatformCreateInfo info) noexcept;
    virtual ~Platform() noexcept;

    virtual std::shared_ptr<Surface> createSurface(void* nativeWindow) = 0;

protected:
    Adapter m_adapter;
};

} // namespace vkt
