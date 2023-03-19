#pragma once

namespace vkt
{

class Device;

struct RenderPassCreateInfo
{
};

class RenderPass
{
public:
    RenderPass() = delete;
    RenderPass(Device* device, RenderPassCreateInfo info);
    virtual ~RenderPass() = default;

protected:
    Device* m_device;
};
} // namespace vkt