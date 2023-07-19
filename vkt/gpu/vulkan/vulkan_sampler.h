#pragma once

#include "utils/cast.h"
#include "vkt/gpu/sampler.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanDevice;
class VulkanSampler : public Sampler
{
public:
    VulkanSampler() = delete;
    VulkanSampler(VulkanDevice* device, const SamplerDescriptor& descriptor);
    ~VulkanSampler() override;
};

DOWN_CAST(VulkanSampler, Sampler);

// Convert Helper
VkSamplerAddressMode ToVkSamplerAddressMode(AddressMode mode);
AddressMode ToAddressMode(VkSamplerAddressMode mode);
VkFilter ToVkFilter(FilterMode mode);
FilterMode ToFilterMode(VkFilter filter);
VkSamplerMipmapMode ToVkSamplerMipmapMode(MipmapFilterMode mode);
MipmapFilterMode ToMipmapFilterMode(VkSamplerMipmapMode mode);

}; // namespace vkt
