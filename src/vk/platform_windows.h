#pragma once

#include "platform.h"

namespace vkt
{
class PlatformWindows : public Platform
{
public:
    std::shared_ptr<Surface> createSurface(void* nativeWindow) override;
};

} // namespace vkt
