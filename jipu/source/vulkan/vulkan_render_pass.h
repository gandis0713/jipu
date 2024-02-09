#pragma once

#include "export.h"

#include "jipu/render_pass_encoder.h"
#include "jipu/texture.h"
#include "vulkan_api.h"

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace jipu
{

struct VulkanColorAttachment
{
    TextureFormat format = TextureFormat::kUndefined;
    LoadOp loadOp = LoadOp::kDontCare;
    StoreOp storeOp = StoreOp::kDontCare;

    // TODO: custom type?
    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // TODO: custom type?
    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};

struct VulkanDepthStencilAttachment
{
    TextureFormat format = TextureFormat::kUndefined;
    LoadOp depthLoadOp = LoadOp::kDontCare;
    StoreOp depthStoreOp = StoreOp::kDontCare;
    LoadOp stencilLoadOp = LoadOp::kDontCare;
    StoreOp stencilStoreOp = StoreOp::kDontCare;
};
struct VulkanRenderPassDescriptor
{
    std::vector<VulkanColorAttachment> colorAttachments{};
    std::optional<VulkanDepthStencilAttachment> depthStencilAttachment = std::nullopt;
    uint32_t sampleCount = 1;
};

class VulkanDevice;
class JIPU_EXPERIMENTAL_EXPORT VulkanRenderPass
{
public:
    VulkanRenderPass(VulkanDevice* device, const VulkanRenderPassDescriptor& descriptor);
    VulkanRenderPass(VulkanDevice* device, const std::vector<VulkanRenderPassDescriptor>& descriptors);
    ~VulkanRenderPass();

    VkRenderPass getVkRenderPass() const;

private:
    VulkanDevice* m_device = nullptr;

private:
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
};

class JIPU_EXPERIMENTAL_EXPORT VulkanRenderPassCache final
{

public:
    VulkanRenderPassCache(VulkanDevice* device);
    ~VulkanRenderPassCache() = default;

    VulkanRenderPass* getRenderPass(const std::vector<VulkanRenderPassDescriptor>& descriptors);

    void clear();

private:
    VulkanDevice* m_device = nullptr;

private:
    struct Functor
    {
        size_t operator()(const std::vector<VulkanRenderPassDescriptor>& descriptors) const;
        bool operator()(const std::vector<VulkanRenderPassDescriptor>& lhs, const std::vector<VulkanRenderPassDescriptor>& rhs) const;
    };
    using Cache = std::unordered_map<std::vector<VulkanRenderPassDescriptor>, std::unique_ptr<VulkanRenderPass>, Functor, Functor>;

    Cache m_cache{};
};

// Convert Helper
VkAttachmentLoadOp ToVkAttachmentLoadOp(LoadOp loadOp);
LoadOp ToVkAttachmentLoadOp(VkAttachmentLoadOp loadOp);
VkAttachmentStoreOp ToVkAttachmentStoreOp(StoreOp storeOp);
StoreOp ToStoreOp(VkAttachmentStoreOp storeOp);

} // namespace jipu
