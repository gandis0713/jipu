#pragma once

#include "gpu/surface.h"

#include <memory>

namespace vkt
{

struct PlatformDescriptor
{
    void* windowHandle;
};

class Adapter;

class Platform
{
public:
    Platform(Adapter* adapter, PlatformDescriptor descriptor) noexcept;
    virtual ~Platform() noexcept = default;

    virtual std::unique_ptr<Surface> createSurface(SurfaceDescriptor descriptor) = 0;

    Adapter* getAdapter() const;

protected:
    Adapter* m_adapter{ nullptr };

protected:
    void* m_windowHandle{ nullptr };
};

} // namespace vkt
