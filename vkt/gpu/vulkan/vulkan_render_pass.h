#pragma once

#include "vulkan_api.h"

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace vkt
{

struct VulkanRenderPassDescriptor
{
    /// @brief Color attachment format. It should be same with the image in swapchain.
    VkFormat colorFormat = VK_FORMAT_UNDEFINED; // TODO: multiple color attachments
    std::optional<VkFormat> depthStencilFormat = std::nullopt;
    VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
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

} // namespace vkt
