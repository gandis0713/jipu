#include "vulkan_binding_group_layout.h"
#include "vulkan_device.h"

#include <fmt/format.h>
#include <stdexcept>

namespace vkt
{

VulkanBindingGroupLayout::VulkanBindingGroupLayout(VulkanDevice* device, const BindingGroupLayoutDescriptor& descriptor)
    : BindingGroupLayout(device, descriptor)
{

    const uint64_t bufferSize = descriptor.buffers.size();
    const uint64_t samplerSize = descriptor.samplers.size();
    const uint64_t textureSize = descriptor.textures.size();
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
    layoutBindings.resize(bufferSize + samplerSize + textureSize);

    for (uint64_t i = 0; i < bufferSize; ++i)
    {
        const auto& buffer = descriptor.buffers[i];
        layoutBindings[i] = { .binding = buffer.index,
                              .descriptorType = ToVkDescriptorType(buffer.type, buffer.dynamicOffset),
                              .descriptorCount = 1,
                              .stageFlags = ToVkShaderStageFlags(buffer.stages),
                              .pImmutableSamplers = nullptr };
    }

    for (uint64_t i = 0; i < samplerSize; ++i)
    {
        const auto& sampler = descriptor.samplers[i];
        layoutBindings[bufferSize + i] = { .binding = sampler.index,
                                           .descriptorType = sampler.withTexture ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLER,
                                           .descriptorCount = 1,
                                           .stageFlags = ToVkShaderStageFlags(sampler.stages),
                                           .pImmutableSamplers = nullptr };
    }

    for (uint64_t i = 0; i < textureSize; ++i)
    {
        const auto& texture = descriptor.textures[i];
        layoutBindings[bufferSize + samplerSize + i] = { .binding = texture.index,
                                                         .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                                         .descriptorCount = 1,
                                                         .stageFlags = ToVkShaderStageFlags(texture.stages),
                                                         .pImmutableSamplers = nullptr };
    }

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                                      .bindingCount = static_cast<uint32_t>(layoutBindings.size()),
                                                      .pBindings = layoutBindings.data() };
    const VulkanAPI& vkAPI = device->vkAPI;
    VkResult result = vkAPI.CreateDescriptorSetLayout(device->getVkDevice(), &layoutCreateInfo, nullptr, &m_descriptorSetLayout);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create VkDescriptorSetLayout");
    }
}

VulkanBindingGroupLayout::~VulkanBindingGroupLayout()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyDescriptorSetLayout(vulkanDevice->getVkDevice(), m_descriptorSetLayout, nullptr);
}

VkDescriptorSetLayout VulkanBindingGroupLayout::getVkDescriptorSetLayout() const
{
    return m_descriptorSetLayout;
}

// Convert Helper
VkDescriptorType ToVkDescriptorType(BufferBindingType type, bool dynamicOffset)
{
    switch (type)
    {
    case BufferBindingType::kUniform:
    {
        if (dynamicOffset)
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        else
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
    case BufferBindingType::kStorage:
        if (dynamicOffset)
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        else
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    default:
    case BufferBindingType::kUndefined:
        throw std::runtime_error(fmt::format("Failed to support type [{}] for VkDescriptorType.", static_cast<int32_t>(type)));
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

BufferBindingType ToBufferBindingType(VkDescriptorType type)
{
    switch (type)
    {
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        return BufferBindingType::kUniform;
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        return BufferBindingType::kStorage;
    default:
        throw std::runtime_error(fmt::format("Failed to support type [{}] for BufferBindingType.", static_cast<int32_t>(type)));
        return BufferBindingType::kUndefined;
    }
}

VkShaderStageFlags ToVkShaderStageFlags(BindingStageFlags flags)
{
    VkShaderStageFlags vkFlags = 0x00000000; // 0x00000000

    if (flags & BindingStageFlagBits::kVertexStage)
    {
        vkFlags |= VK_SHADER_STAGE_VERTEX_BIT;
    }
    if (flags & BindingStageFlagBits::kFragmentStage)
    {
        vkFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    if (flags & BindingStageFlagBits::kComputeStage)
    {
        vkFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
    }

    return vkFlags;
}

BindingStageFlags ToBindingStageFlags(VkShaderStageFlags vkFlags)
{
    BindingStageFlags flags = 0u;

    if (flags & VK_SHADER_STAGE_VERTEX_BIT)
    {
        vkFlags |= BindingStageFlagBits::kVertexStage;
    }
    if (flags & VK_SHADER_STAGE_FRAGMENT_BIT)
    {
        vkFlags |= BindingStageFlagBits::kFragmentStage;
    }
    if (flags & VK_SHADER_STAGE_COMPUTE_BIT)
    {
        vkFlags |= BindingStageFlagBits::kComputeStage;
    }

    return flags;
}

} // namespace vkt