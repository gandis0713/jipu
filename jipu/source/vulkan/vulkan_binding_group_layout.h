#pragma once

#include "jipu/binding_group_layout.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
{

enum class VulkanTextureBindingType
{
    kTextureBinding = 0,
    kInputAttachment
};

struct VulkanBufferBindingLayout
{
};

struct VulkanSamplerBindingLayout
{
};

struct VulkanTextureBindingLayout
{
    VulkanTextureBindingType type;
};
struct VulkanBindingGroupLayoutDescriptor
{
    std::vector<VulkanBufferBindingLayout> buffers{};
    std::vector<VulkanSamplerBindingLayout> samplers{};
    std::vector<VulkanTextureBindingLayout> textures{};
};

class VulkanDevice;
class JIPU_EXPERIMENTAL_EXPORT VulkanBindingGroupLayout : public BindingGroupLayout
{
public:
    VulkanBindingGroupLayout() = delete;
    VulkanBindingGroupLayout(VulkanDevice* device, const BindingGroupLayoutDescriptor& descriptor, const VulkanBindingGroupLayoutDescriptor& vkdescriptor);
    ~VulkanBindingGroupLayout() override;

    const std::vector<BufferBindingLayout>& getBufferBindingLayouts() const;
    std::optional<BufferBindingLayout> getBufferBindingLayout(uint32_t index) const;

    const std::vector<SamplerBindingLayout>& getSamplerBindingLayouts() const;
    std::optional<SamplerBindingLayout> getSamplerBindingLayout(uint32_t index) const;

    const std::vector<TextureBindingLayout>& getTextureBindingLayouts() const;
    std::optional<TextureBindingLayout> getTextureBindingLayout(uint32_t index) const;

    VkDescriptorSetLayout getVkDescriptorSetLayout() const;

private:
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

private:
    VulkanDevice* m_device = nullptr;
    const BindingGroupLayoutDescriptor m_descriptor{};
    const VulkanBindingGroupLayoutDescriptor m_vkdescriptor{};
};
DOWN_CAST(VulkanBindingGroupLayout, BindingGroupLayout);

// Convert Helper
VkDescriptorType ToVkDescriptorType(BufferBindingType type, bool dynamicOffset = false);
VkDescriptorType ToVkDescriptorType(VulkanTextureBindingType type);
BufferBindingType ToBufferBindingType(VkDescriptorType type);
VkShaderStageFlags ToVkShaderStageFlags(BindingStageFlags flags);
BindingStageFlags ToBindingStageFlags(VkShaderStageFlags flags);
} // namespace jipu