#include "vulkan_binding_group_layout.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanBindingGroupLayout::VulkanBindingGroupLayout(VulkanDevice* device, const BindingGroupLayoutDescriptor& descriptor)
    : BindingGroupLayout(device, descriptor)
{
    // const uint32_t& bindingCount = descriptor.bindings.size();
    // std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
    // layoutBindings.resize(bindingCount);
    // for (uint32_t i = 0; i < bindingCount; ++i)
    // {
    //     const auto& binding = descriptor.bindings[i];
    //     layoutBindings[i] = { .binding = binding.index,
    //                           .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // TODO: need from descriptor
    //                           .descriptorCount = 1,
    //                           .stageFlags = VK_SHADER_STAGE_VERTEX_BIT, // TODO: need from descriptor
    //                           .pImmutableSamplers = nullptr };
    // }

    // VkDescriptorSetLayoutCreateInfo layoutCreateInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    //                                                   .bindingCount = static_cast<uint32_t>(layoutBindings.size()),
    //                                                   .pBindings = layoutBindings.data() };
    // const VulkanAPI& vkAPI = device->vkAPI;
    // VkResult result = vkAPI.CreateDescriptorSetLayout(device->getVkDevice(), &layoutCreateInfo, nullptr, &m_descriptorSetLayout);
    // if (result != VK_SUCCESS)
    // {
    //     throw std::runtime_error("Failed to create VkDescriptorSetLayout");
    // }
}

VulkanBindingGroupLayout::~VulkanBindingGroupLayout()
{
    // auto vulkanDevice = downcast(m_device);
    // vulkanDevice->vkAPI.DestroyDescriptorSetLayout(vulkanDevice->getVkDevice(), m_descriptorSetLayout, nullptr);
}

VkDescriptorSetLayout VulkanBindingGroupLayout::getVkDescriptorSetLayout() const
{
    return m_descriptorSetLayout;
}
} // namespace vkt