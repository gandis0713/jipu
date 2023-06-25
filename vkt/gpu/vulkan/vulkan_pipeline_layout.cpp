#include "vulkan_pipeline_layout.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanBindingLayout::VulkanBindingLayout(VulkanDevice* device, const BindingLayoutDescriptor& descriptor)
    : BindingLayout(device, descriptor)
{
    const uint32_t& bindingCount = descriptor.bindings.size();
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
    layoutBindings.resize(bindingCount);
    for (uint32_t i = 0; i < bindingCount; ++i)
    {
        const auto& binding = descriptor.bindings[i];
        layoutBindings[i] = { .binding = binding.index,
                              .descriptorCount = 1,
                              .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // TODO: need from descriptor
                              .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,            // TODO: need from descriptor
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

VulkanBindingLayout::~VulkanBindingLayout()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyDescriptorSetLayout(vulkanDevice->getVkDevice(), m_descriptorSetLayout, nullptr);
}

VkDescriptorSetLayout VulkanBindingLayout::getVkDescriptorSetLayout() const
{
    return m_descriptorSetLayout;
}

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* device, const PipelineLayoutDescriptor& descriptor)
    : PipelineLayout(device, descriptor)
{
    std::vector<VkDescriptorSetLayout> layouts{};
    layouts.resize(descriptor.layouts.size());
    for (uint32_t i = 0; i < descriptor.layouts.size(); ++i)
    {
        layouts[i] = downcast(descriptor.layouts[i])->getVkDescriptorSetLayout();
    }

    VkPipelineLayoutCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                           .setLayoutCount = static_cast<uint32_t>(layouts.size()),
                                           .pSetLayouts = layouts.data() };

    VkResult result = device->vkAPI.CreatePipelineLayout(device->getVkDevice(), &createInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create VkPipelineLayout");
    }
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyPipelineLayout(vulkanDevice->getVkDevice(), m_pipelineLayout, nullptr);
}

VkPipelineLayout VulkanPipelineLayout::getVkPipelineLayout() const
{
    return m_pipelineLayout;
}

} // namespace vkt