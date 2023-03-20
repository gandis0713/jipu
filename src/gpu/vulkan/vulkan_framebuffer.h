#pragma once

#include "gpu/framebuffer.h"

namespace vkt
{

class VulkanDevice;

class VulkanFrameBuffer : public FrameBuffer
{
public:
    VulkanFrameBuffer() = delete;
    VulkanFrameBuffer(VulkanDevice* device, FramebufferCreateInfo info);
    ~VulkanFrameBuffer() override = default;
};

} // namespace vkt