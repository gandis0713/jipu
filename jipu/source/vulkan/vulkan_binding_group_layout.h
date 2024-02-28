#pragma once

#include "jipu/binding_group_layout.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
{

struct VulkanBindingGroupLayoutDescriptor
{
    const void* next = nullptr;
    VkDescriptorSetLayoutCreateFlags flags = 0u;
    std::vector<VkDescriptorSetLayoutBinding> buffers{};
    std::vector<VkDescriptorSetLayoutBinding> samplers{};
    std::vector<VkDescriptorSetLayoutBinding> textures{};
};

class VulkanDevice;
class JIPU_EXPERIMENTAL_EXPORT VulkanBindingGroupLayout : public BindingGroupLayout
{
public:
    VulkanBindingGroupLayout() = delete;
    VulkanBindingGroupLayout(VulkanDevice* device, const BindingGroupLayoutDescriptor& descriptor);
    VulkanBindingGroupLayout(VulkanDevice* device, const VulkanBindingGroupLayoutDescriptor& descriptor);
    ~VulkanBindingGroupLayout() override;

    // const std::vector<BufferBindingLayout>& getBufferBindingLayouts() const;
    // std::optional<BufferBindingLayout> getBufferBindingLayout(uint32_t index) const;

    // const std::vector<SamplerBindingLayout>& getSamplerBindingLayouts() const;
    // std::optional<SamplerBindingLayout> getSamplerBindingLayout(uint32_t index) const;

    // const std::vector<TextureBindingLayout>& getTextureBindingLayouts() const;
    // std::optional<TextureBindingLayout> getTextureBindingLayout(uint32_t index) const;

    std::vector<VkDescriptorSetLayoutBinding> getBufferBindingLayouts() const;
    VkDescriptorSetLayoutBinding getBufferBindingLayout(uint32_t index) const;

    std::vector<VkDescriptorSetLayoutBinding> getSamplerBindingLayouts() const;
    VkDescriptorSetLayoutBinding getSamplerBindingLayout(uint32_t index) const;

    std::vector<VkDescriptorSetLayoutBinding> getTextureBindingLayouts() const;
    VkDescriptorSetLayoutBinding getTextureBindingLayout(uint32_t index) const;

    VkDescriptorSetLayout getVkDescriptorSetLayout() const;

private:
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

private:
    VulkanDevice* m_device = nullptr;
    const VulkanBindingGroupLayoutDescriptor m_descriptor{};
};
DOWN_CAST(VulkanBindingGroupLayout, BindingGroupLayout);

// Convert Helper
VkDescriptorType ToVkDescriptorType(BufferBindingType type, bool dynamicOffset = false);
BufferBindingType ToBufferBindingType(VkDescriptorType type);
VkShaderStageFlags ToVkShaderStageFlags(BindingStageFlags flags);
BindingStageFlags ToBindingStageFlags(VkShaderStageFlags flags);
} // namespace jipu