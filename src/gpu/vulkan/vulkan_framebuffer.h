#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "vulkan_api.h"

namespace vkt
{

struct VulkanFramebufferDescriptor
{
    VkRenderPass renderPass{ nullptr };
    std::vector<VkImageView> imageViews{};

    uint32_t width{ 0 };
    uint32_t height{ 0 };
};

class VulkanDevice;

class VulkanFrameBuffer
{
public:
    VulkanFrameBuffer() = delete;
    VulkanFrameBuffer(VulkanDevice* device, const VulkanFramebufferDescriptor& descriptor);
    ~VulkanFrameBuffer() = default;

    VkFramebuffer getVkFrameBuffer() const;

private:
    VulkanDevice* m_device{ nullptr };

private:
    VkFramebuffer m_framebuffer{ VK_NULL_HANDLE };
};

class VulkanFrameBufferCache final
{

public:
    VulkanFrameBufferCache(VulkanDevice* device);
    ~VulkanFrameBufferCache() = default;

    VulkanFrameBuffer* getFrameBuffer(const VulkanFramebufferDescriptor& descriptor);

private:
    VulkanDevice* m_device{ nullptr };

private:
    struct Functor
    {
        size_t operator()(const VulkanFramebufferDescriptor& descriptor) const;
        bool operator()(const VulkanFramebufferDescriptor& lhs, const VulkanFramebufferDescriptor& rhs) const;
    };
    using Cache = std::unordered_map<VulkanFramebufferDescriptor, std::unique_ptr<VulkanFrameBuffer>, Functor, Functor>;

    Cache m_cache{};
};

} // namespace vkt
