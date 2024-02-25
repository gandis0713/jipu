#include "vulkan_binding_group_layout.h"
#include "vulkan_device.h"

#include <fmt/format.h>
#include <stdexcept>

namespace jipu
{

VulkanBindingGroupLayout::VulkanBindingGroupLayout(VulkanDevice* device, const BindingGroupLayoutDescriptor& descriptor, const VulkanBindingGroupLayoutDescriptor& vkdescriptor)
    : m_device(device)
    , m_descriptor(descriptor)
    , m_vkdescriptor(vkdescriptor)
{
    if (descriptor.buffers.size() != vkdescriptor.buffers.size())
        throw std::runtime_error("Failed to invalid descriptors for buffer.");

    if (descriptor.samplers.size() != vkdescriptor.samplers.size())
        throw std::runtime_error("Failed to invalid descriptors for sampler.");

    if (descriptor.textures.size() != vkdescriptor.textures.size())
        throw std::runtime_error("Failed to invalid descriptors for texture.");

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
                                           .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                                           .descriptorCount = 1,
                                           .stageFlags = ToVkShaderStageFlags(sampler.stages),
                                           .pImmutableSamplers = nullptr };
    }

    for (uint64_t i = 0; i < textureSize; ++i)
    {
        const auto& texture = descriptor.textures[i];
        const auto& vktexture = vkdescriptor.textures[i];
        layoutBindings[bufferSize + samplerSize + i] = { .binding = texture.index,
                                                         .descriptorType = ToVkDescriptorType(vktexture.type),
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

const std::vector<BufferBindingLayout>& VulkanBindingGroupLayout::getBufferBindingLayouts() const
{
    return m_descriptor.buffers;
}

std::optional<BufferBindingLayout> VulkanBindingGroupLayout::getBufferBindingLayout(uint32_t index) const
{
    for (const auto& buffer : m_descriptor.buffers)
    {
        if (buffer.index == index)
            return buffer;
    }
    return std::nullopt;
}

const std::vector<SamplerBindingLayout>& VulkanBindingGroupLayout::getSamplerBindingLayouts() const
{
    return m_descriptor.samplers;
}

std::optional<SamplerBindingLayout> VulkanBindingGroupLayout::getSamplerBindingLayout(uint32_t index) const
{
    for (const auto& sampler : m_descriptor.samplers)
    {
        if (sampler.index == index)
            return sampler;
    }
    return std::nullopt;
}

const std::vector<TextureBindingLayout>& VulkanBindingGroupLayout::getTextureBindingLayouts() const
{
    return m_descriptor.textures;
}

std::optional<TextureBindingLayout> VulkanBindingGroupLayout::getTextureBindingLayout(uint32_t index) const
{
    for (const auto& texture : m_descriptor.textures)
    {
        if (texture.index == index)
            return texture;
    }
    return std::nullopt;
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
    case BufferBindingType::kUniform: {
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

VkDescriptorType ToVkDescriptorType(const VulkanTextureBindingType type)
{
    switch (type)
    {
    case VulkanTextureBindingType::kInputAttachment:
        return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    default:
    case VulkanTextureBindingType::kTextureBinding:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
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

} // namespace jipu