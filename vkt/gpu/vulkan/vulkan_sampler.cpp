#include "vulkan_sampler.h"
#include "vulkan_device.h"

#include <fmt/format.h>
#include <stdexcept>

namespace vkt
{

VulkanSampler::VulkanSampler(VulkanDevice* device, const SamplerDescriptor& descriptor)
    : Sampler(device, descriptor)
{
}

VulkanSampler::~VulkanSampler()
{
}

// Convert Helper
VkSamplerAddressMode ToVkSamplerAddressMode(AddressMode mode)
{
    switch (mode)
    {
    case AddressMode::kClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case AddressMode::kRepeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case AddressMode::kMirrorRepeat:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    }
}

AddressMode ToAddressMode(VkSamplerAddressMode mode)
{
    switch (mode)
    {
    case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
        return AddressMode::kClampToEdge;
    case VK_SAMPLER_ADDRESS_MODE_REPEAT:
        return AddressMode::kRepeat;
    case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
        return AddressMode::kMirrorRepeat;
    default:
        throw std::runtime_error(fmt::format("{} mode does not support.", static_cast<int32_t>(mode)));
        return AddressMode::kClampToEdge;
    }
}

VkFilter ToVkFilter(FilterMode mode)
{
    switch (mode)
    {
    case FilterMode::kNearest:
        return VK_FILTER_NEAREST;
    case FilterMode::kLinear:
        return VK_FILTER_LINEAR;
    }
}

FilterMode ToFilterMode(VkFilter filter)
{
    switch (filter)
    {
    case VK_FILTER_NEAREST:
        return FilterMode::kNearest;
    case VK_FILTER_LINEAR:
        return FilterMode::kLinear;
    default:
        throw std::runtime_error(fmt::format("{} mode does not support.", static_cast<int32_t>(filter)));
        return FilterMode::kNearest;
    }
}

VkSamplerMipmapMode ToVkSamplerMipmapMode(MipmapFilterMode mode)
{
    switch (mode)
    {
    case MipmapFilterMode::kNearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case MipmapFilterMode::kLinear:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

MipmapFilterMode ToMipmapFilterMode(VkSamplerMipmapMode mode)
{
    switch (mode)
    {
    case VK_SAMPLER_MIPMAP_MODE_NEAREST:
        return MipmapFilterMode::kNearest;
    case VK_SAMPLER_MIPMAP_MODE_LINEAR:
        return MipmapFilterMode::kLinear;
    default:
        throw std::runtime_error(fmt::format("{} mode does not support.", static_cast<int32_t>(mode)));
        return MipmapFilterMode::kNearest;
    }
}

}; // namespace vkt
