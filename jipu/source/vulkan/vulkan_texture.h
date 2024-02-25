#pragma once

#include "export.h"

#include "jipu/texture.h"
#include "utils/assert.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_resource.h"
#include "vulkan_texture_view.h"

#include <fmt/format.h>

namespace jipu
{

enum class TextureOwner
{
    External,
    Internal // by swap chain
};

enum VulkanTextureUsageFlagBits
{
    kUndefined = 0x00000000,
    kInputAttachment = 0x00000001,
};
using VulkanTextureUsageFlags = uint32_t;

struct VulkanTextureDescriptor
{
    VkImage image = VK_NULL_HANDLE;
    VulkanTextureUsageFlags usages = VulkanTextureUsageFlagBits::kUndefined;
};

class VulkanDevice;
class JIPU_EXPERIMENTAL_EXPORT VulkanTexture : public Texture
{
public:
    VulkanTexture() = delete;
    VulkanTexture(VulkanDevice* device, const TextureDescriptor& descriptor, const VulkanTextureDescriptor& vkdescriptor);
    ~VulkanTexture() override;

    std::unique_ptr<TextureView> createTextureView(const TextureViewDescriptor& descriptor) override;

public:
    TextureType getType() const override;
    TextureFormat getFormat() const override;
    TextureUsageFlags getUsage() const override;
    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    uint32_t getDepth() const override;
    uint32_t getMipLevels() const override;
    uint32_t getSampleCount() const override;

public:
    VulkanDevice* getDevice() const;
    VulkanTextureUsageFlags getVulkanTextureUsages() const;

public:
    VkImage getVkImage() const;
    TextureOwner getTextureOwner() const;

    /// @brief record pipeline barrier command, but not submitted.
    void setPipelineBarrier(VkCommandBuffer commandBuffer, VkImageLayout layout, VkImageSubresourceRange range);

    /// @brief return current VkImageLayout if set pipeline barrier by layout and surbresource range.
    /// @return VkImageLayout
    VkImageLayout getLayout() const;

    /// @brief generate final layout and return it.
    /// @return VKImageLayout
    VkImageLayout getFinalLayout() const;

private:
    VulkanDevice* m_device = nullptr;
    const TextureDescriptor m_descriptor{};
    const VulkanTextureDescriptor m_vkdescriptor{};

private:
    VulkanTextureResource m_resource;
    TextureOwner m_owner;

private:
    VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

DOWN_CAST(VulkanTexture, Texture);

// Convert Helper
VkFormat ToVkFormat(TextureFormat format);
TextureFormat ToTextureFormat(VkFormat format);
VkImageType ToVkImageType(TextureType type);
TextureType ToTextureType(VkImageType type);
VkImageUsageFlags ToVkImageUsageFlags(TextureUsageFlags usages, VulkanTextureUsageFlags usage);
TextureUsageFlags ToTextureUsageFlags(VkImageUsageFlags usages);
VkSampleCountFlagBits ToVkSampleCountFlagBits(uint32_t count);

// Utils
VkImageLayout GenerateFinalImageLayout(TextureUsageFlags usage);
VkAccessFlags GenerateAccessFlags(VkImageLayout layout);
VkPipelineStageFlags GeneratePipelineStage(VkImageLayout layout);

} // namespace jipu
