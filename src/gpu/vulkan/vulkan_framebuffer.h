#pragma once

namespace vkt
{

struct FramebufferCreateInfo
{
};

class VulkanDevice;

class VulkanFrameBuffer
{
public:
    VulkanFrameBuffer() = delete;
    VulkanFrameBuffer(VulkanDevice* device, FramebufferCreateInfo descriptor);
    ~VulkanFrameBuffer() = default;
};

} // namespace vkt