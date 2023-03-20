#pragma once

#include "gpu/surface.h"

#include <memory>

namespace vkt
{

struct PlatformCreateInfo
{
    void* windowHandle;
};

class Adapter;

class Platform
{
public:
    Platform(Adapter* adapter, PlatformCreateInfo info) noexcept;
    virtual ~Platform() noexcept = default;

    virtual std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) = 0;

    Adapter* getAdapter() const;

protected:
    Adapter* m_adapter{ nullptr };

protected:
    void* m_windowHandle{ nullptr };
};

} // namespace vkt
