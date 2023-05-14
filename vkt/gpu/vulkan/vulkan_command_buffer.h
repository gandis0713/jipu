#pragma once

#include "vkt/gpu/command_buffer.h"

namespace vkt
{

class VulkanCommandEncoder;
class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(VulkanCommandEncoder* encoder, const CommandBufferDescriptor& descriptor);
    ~VulkanCommandBuffer() override = default;
};
} // namespace vkt