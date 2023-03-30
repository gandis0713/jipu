#pragma once

namespace vkt
{

class Device;

struct RenderPassDescriptor
{
};

class RenderPass
{
public:
    RenderPass() = delete;
    RenderPass(Device* device, RenderPassDescriptor descriptor);
    virtual ~RenderPass() = default;

protected:
    Device* m_device;
};
} // namespace vkt
