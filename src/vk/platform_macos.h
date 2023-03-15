#pragma once

#include "platform.h"

namespace vkt
{

class PlatformMacOS : public Platform
{
public:
    PlatformMacOS(PlatformCreateInfo info);
    ~PlatformMacOS() override;

    std::shared_ptr<Surface> createSurface(void* nativeWindow) override;
};

} // namespace vkt
