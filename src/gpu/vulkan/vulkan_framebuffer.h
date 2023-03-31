#pragma once

#include <memory>
#include <unordered_map>

#include "vulkan_api.h"

namespace vkt
{

struct VulkanFramebufferDescriptor
{
    VkRenderPass renderPass;
};

class VulkanDevice;

class VulkanFrameBuffer
{
public:
    VulkanFrameBuffer() = delete;
    VulkanFrameBuffer(VulkanDevice* device, VulkanFramebufferDescriptor descriptor);
    ~VulkanFrameBuffer() = default;

private:
    VulkanDevice* m_device{ nullptr };
};

class VulkanFrameBufferCache final
{

public:
    VulkanFrameBufferCache(VulkanDevice* device);
    ~VulkanFrameBufferCache() = default;

    VulkanFrameBuffer* getFrameBuffer(VulkanFramebufferDescriptor descriptor);

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
