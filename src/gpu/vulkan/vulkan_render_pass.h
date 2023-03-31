#pragma once

#include "vulkan_api.h"

#include <memory>
#include <unordered_map>

namespace vkt
{

struct VulkanRenderPassDescriptor
{
    /// @brief Color attachment format. It should be same with the image in swapchain.
    VkFormat format{ VK_FORMAT_UNDEFINED };
    VkSampleCountFlagBits samples{ VK_SAMPLE_COUNT_1_BIT };

    VkAttachmentLoadOp loadOp{ VK_ATTACHMENT_LOAD_OP_CLEAR };
    VkAttachmentStoreOp storeOp{ VK_ATTACHMENT_STORE_OP_STORE };
};

class VulkanDevice;
class VulkanRenderPass
{
public:
    VulkanRenderPass(VulkanDevice* device, VulkanRenderPassDescriptor descriptor);
    ~VulkanRenderPass();

    VkRenderPass getRenderPass() const;

private:
    VulkanDevice* m_device{ nullptr };

private:
    VkRenderPass m_renderPass{ nullptr };
};

class VulkanRenderPassCache final
{

public:
    VulkanRenderPassCache(VulkanDevice* device);
    ~VulkanRenderPassCache();

    VulkanRenderPass* getRenderPass(const VulkanRenderPassDescriptor& descriptor);

    void clear();

private:
    VulkanDevice* m_device{ nullptr };

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
