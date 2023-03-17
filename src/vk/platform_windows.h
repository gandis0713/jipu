#pragma once

#include "platform.h"

namespace vkt
{
class PlatformWindows : public Platform
{
public:
    PlatformWindows(PlatformCreateHandles handles, PlatformCreateInfo info) noexcept;
    ~PlatformWindows() override;

    std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) override;
};

} // namespace vkt
