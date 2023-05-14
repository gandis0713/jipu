#pragma once

#include "vkt/gpu/command_encoder.h"

namespace vkt
{

class VulkanCommandBuffer;
class VulkanCommandEncoder : public CommandEncoder
{
public:
    VulkanCommandEncoder() = delete;
    VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor);
    ~VulkanCommandEncoder() override = default;
};
} // namespace vkt