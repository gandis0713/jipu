#pragma once

#include "platform.h"

namespace vkt
{
class PlatformWindows : public Platform
{
    void* createSurface(void* nativeWindow, void* instance) override;
};

} // namespace vkt