#include "vulkan_binding_group_layout.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanBindingGroupLayout::VulkanBindingGroupLayout(VulkanDevice* device, const BindingGroupLayoutDescriptor& descriptor)
    : BindingGroupLayout(device, descriptor)
{

    const uint64_t& bufferSize = descriptor.buffers.size();
    const uint64_t& textureSize = descriptor.textures.size();
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
    layoutBindings.resize(bufferSize + textureSize);
    for (uint32_t i = 0; i < bufferSize; ++i)
    {
        const auto& buffer = descriptor.buffers[i];
        layoutBindings[i] = { .binding = buffer.index,
                              .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // TODO: need from descriptor
                              .descriptorCount = 1,
                              .stageFlags = VK_SHADER_STAGE_VERTEX_BIT, // TODO: need from descriptor
                              .pImmutableSamplers = nullptr };
    }

    for (uint32_t i = bufferSize; i < bufferSize + textureSize; ++i)
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

} // namespace vkt