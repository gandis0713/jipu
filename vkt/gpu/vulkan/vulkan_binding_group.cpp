#include "vulkan_binding_group.h"
#include "vulkan_binding_group_layout.h"
#include "vulkan_buffer.h"
#include "vulkan_device.h"

#include <stdexcept>

namespace vkt
{

VulkanBindingGroup::VulkanBindingGroup(VulkanDevice* device, const BindingGroupDescriptor& descriptor)
    : BindingGroup(device, descriptor)
{
    auto descriptorSetLayout = downcast(descriptor.layout)->getVkDescriptorSetLayout();
    auto vulkanDevice = downcast(device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = downcast(device)->getDescriptorPool();
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

    VkResult result = vkAPI.AllocateDescriptorSets(vulkanDevice->getVkDevice(), &descriptorSetAllocateInfo, &m_descriptorSet);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate descriptor sets.");
    }

    for (const auto buffer : descriptor.buffers)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = downcast(buffer.binding.buffer)->getVkBuffer();
        bufferInfo.offset = buffer.binding.offset;
        bufferInfo.range = buffer.binding.size;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_descriptorSet;
        descriptorWrite.dstBinding = buffer.index;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // TODO: use from descriptor.
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;       // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkAPI.UpdateDescriptorSets(vulkanDevice->getVkDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

VulkanBindingGroup::~VulkanBindingGroup()
{
    auto vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    vkAPI.FreeDescriptorSets(vulkanDevice->getVkDevice(), vulkanDevice->getDescriptorPool(), 1, &m_descriptorSet);
}

VkDescriptorSet VulkanBindingGroup::getVkDescriptorSet() const
{
    return m_descriptorSet;
}

} // namespace vkt