#pragma once

#include "platform.h"

namespace vkt
{

class PlatformMacOS : public Platform
{
public:
    PlatformMacOS(PlatformCreateInfo info);
    ~PlatformMacOS() override;

    void* createVkSurfaceKHR(void* nativeWindow, void* instance) override;
};

} // namespace vkt
