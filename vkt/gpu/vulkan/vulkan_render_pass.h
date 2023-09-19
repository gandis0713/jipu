#pragma once

#include "vkt/gpu/render_pass_encoder.h"
#include "vkt/gpu/texture.h"
#include "vulkan_api.h"

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace vkt
{

struct VulkanColorAttachment
{
    TextureFormat format = TextureFormat::kUndefined;
    LoadOp loadOp = LoadOp::kDontCare;
    StoreOp storeOp = StoreOp::kDontCare;
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
class VulkanRenderPass
{
public:
    VulkanRenderPass(VulkanDevice* device, VulkanRenderPassDescriptor descriptor);
    ~VulkanRenderPass();

    VkRenderPass getVkRenderPass() const;

private:
    VulkanDevice* m_device = nullptr;

private:
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
};

class VulkanRenderPassCache final
{

public:
    VulkanRenderPassCache(VulkanDevice* device);
    ~VulkanRenderPassCache() = default;

    VulkanRenderPass* getRenderPass(const VulkanRenderPassDescriptor& descriptor);

    void clear();

private:
    VulkanDevice* m_device = nullptr;

private:
    struct Functor
    {
        size_t operator()(const VulkanRenderPassDescriptor& descriptor) const;
        bool operator()(const VulkanRenderPassDescriptor& lhs, const VulkanRenderPassDescriptor& rhs) const;
    };
    using Cache = std::unordered_map<VulkanRenderPassDescriptor, std::unique_ptr<VulkanRenderPass>, Functor, Functor>;

    Cache m_cache{};
};

// Convert Helper
VkAttachmentLoadOp ToVkAttachmentLoadOp(LoadOp loadOp);
LoadOp ToVkAttachmentLoadOp(VkAttachmentLoadOp loadOp);
VkAttachmentStoreOp ToVkAttachmentStoreOp(StoreOp storeOp);
StoreOp ToStoreOp(VkAttachmentStoreOp storeOp);

} // namespace vkt
