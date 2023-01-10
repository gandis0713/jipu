#pragma once

#include "vk/context.h"
#include "vk/platform.h"
#include "vk/visibility.h"

namespace vkt
{

class VKT_EXPORT Driver
{
public:
    explicit Driver(Platform* platform = nullptr);
    ~Driver();

    void terminate();

private:
    Context m_context{};
    Platform& m_platform;
};

} // namespace vkt
