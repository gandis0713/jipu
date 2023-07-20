#include "vulkan_binding_group_layout.h"
#include "vulkan_device.h"

#include <fmt/format.h>
#include <stdexcept>

namespace vkt
{

VulkanBindingGroupLayout::VulkanBindingGroupLayout(VulkanDevice* device, const BindingGroupLayoutDescriptor& descriptor)
    : BindingGroupLayout(device, descriptor)
{

    const uint64_t& bufferSize = descriptor.buffers.size();
    const uint64_t& samplerSize = descriptor.samplers.size();
    const uint64_t& textureSize = descriptor.textures.size();
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
    layoutBindings.resize(bufferSize + samplerSize + textureSize);

    for (uint64_t i = 0; i < bufferSize; ++i)
    {
        const auto& buffer = descriptor.buffers[i];
        layoutBindings[i] = { .binding = buffer.index,
                              .descriptorType = ToVkDescriptorType(buffer.type),
                              .descriptorCount = 1,
                              .stageFlags = ToVkShaderStageFlags(buffer.stages),
                              .pImmutableSamplers = nullptr };
    }

    for (uint64_t i = 0; i < samplerSize; ++i)
    {
        const auto& sampler = descriptor.samplers[i];
        layoutBindings[bufferSize + i] = { .binding = sampler.index,
                                           .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, // TODO: use from descriptor
                                           .descriptorCount = 1,
                                           .stageFlags = ToVkShaderStageFlags(sampler.stages),
                                           .pImmutableSamplers = nullptr };
    }

    for (uint64_t i = 0; i < textureSize; ++i)
    {
        // TODO: for texture
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
VkDescriptorType ToVkDescriptorType(BufferBindingType type)
{
    switch (type)
    {
    case BufferBindingType::kUniform:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case BufferBindingType::kStorage:
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
    else if (flags & BindingStageFlagBits::kFragmentStage)
    {
        vkFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    else if (flags & BindingStageFlagBits::kComputeStage)
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
    else if (flags & VK_SHADER_STAGE_FRAGMENT_BIT)
    {
        vkFlags |= BindingStageFlagBits::kFragmentStage;
    }
    else if (flags & VK_SHADER_STAGE_COMPUTE_BIT)
    {
        vkFlags |= BindingStageFlagBits::kComputeStage;
    }

    return flags;
}

} // namespace vkt