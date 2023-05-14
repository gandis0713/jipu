#include "vulkan_command_encoder.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanCommandEncoder::VulkanCommandEncoder(VulkanDevice* device, const CommandEncoderDescriptor& descriptor)
    : CommandEncoder(device, descriptor)
{
}

} // namespace vkt