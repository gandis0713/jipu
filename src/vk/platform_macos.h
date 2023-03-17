#pragma once

#include "platform.h"

namespace vkt
{

class PlatformMacOS : public Platform
{
public:
    PlatformMacOS(PlatformCreateHandles handles, PlatformCreateInfo info) noexcept;
    ~PlatformMacOS() override;

    std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) override;
};

} // namespace vkt
