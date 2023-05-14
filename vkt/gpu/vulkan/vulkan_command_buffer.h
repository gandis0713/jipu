#pragma once

#include "vkt/gpu/command_buffer.h"

namespace vkt
{

class VulkanDevice;
class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(VulkanDevice* device, const CommandBufferDescriptor& descriptor);
    ~VulkanCommandBuffer() override = default;
};
} // namespace vkt