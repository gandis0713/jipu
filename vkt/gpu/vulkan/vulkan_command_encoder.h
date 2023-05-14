#pragma once

#include "vkt/gpu/command_encoder.h"

namespace vkt
{

class VulkanDevice;
class VulkanCommandEncoder : public CommandEncoder
{
public:
    VulkanCommandEncoder() = delete;
    VulkanCommandEncoder(VulkanDevice* device, const CommandEncoderDescriptor& descriptor);
    ~VulkanCommandEncoder() override = default;
};
} // namespace vkt