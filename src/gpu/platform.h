#pragma once

#include "gpu/surface.h"

#include <memory>

namespace vkt
{

struct PlatformCreateInfo
{
    void* windowHandle;
};

class Platform
{
public:
    Platform(PlatformCreateInfo info) noexcept;
    virtual ~Platform() noexcept = default;

    virtual std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) = 0;

protected:
    void* m_windowHandle{ nullptr };
};

} // namespace vkt
