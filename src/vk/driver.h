#pragma once

#include "vk/context.h"
#include "vk/platform.h"

namespace vkt
{

class Driver
{
public:
    Driver(Platform* platform);

private:
    Context m_context;
    Platform& m_platform;
};

} // namespace vkt
