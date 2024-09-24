#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "vulkan_api.h"
#include "vulkan_export.h"
#include "vulkan_render_pass.h"

namespace jipu
{

struct VulkanFramebufferDescriptor
{
    const void* next = nullptr;
    VkFramebufferCreateFlags flags = 0u;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    std::vector<VkImageView> attachments{};
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t layers = 0;
};

class VulkanDevice;
class VULKAN_EXPORT VulkanFramebuffer
{
public:
    VulkanFramebuffer() = delete;
    VulkanFramebuffer(VulkanDevice& device, const VulkanFramebufferDescriptor& descriptor);
    ~VulkanFramebuffer();

    VkFramebuffer getVkFrameBuffer() const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;

private:
    VulkanDevice& m_device;
    const VulkanFramebufferDescriptor m_descriptor{};

private:
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
};

class VULKAN_EXPORT VulkanFramebufferCache final
{

public:
    VulkanFramebufferCache(VulkanDevice& device);
    ~VulkanFramebufferCache() = default;

    VulkanFramebuffer* getFrameBuffer(const VulkanFramebufferDescriptor& descriptor);

    void clear();

private:
    VulkanDevice& m_device;

private:
    struct Functor
    {
        // hash key
        size_t operator()(const VulkanFramebufferDescriptor& descriptor) const;
        // equal
        bool operator()(const VulkanFramebufferDescriptor& lhs, const VulkanFramebufferDescriptor& rhs) const;
    };
    using Cache = std::unordered_map<VulkanFramebufferDescriptor, std::unique_ptr<VulkanFramebuffer>, Functor, Functor>;

    Cache m_cache{};
};

} // namespace jipu
