#pragma once

#include "vk/context.h"
#include "vk/platform.h"

namespace vkt
{

class Driver
{
public:
    explicit Driver(Platform* platform = nullptr);
    ~Driver();

private:
    void terminate();

private:
    Context m_context;
    Platform& m_platform;
};

} // namespace vkt
