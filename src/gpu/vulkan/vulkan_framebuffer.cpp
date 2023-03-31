#include "vulkan_framebuffer.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"

#include "utils/hash.h"

namespace vkt
{

VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, VulkanFramebufferDescriptor descriptor)
    : m_device(device)
{
    // TODO
}

size_t VulkanFrameBufferCache::Functor::operator()(const VulkanFramebufferDescriptor& descriptor) const
{
    size_t hash = vkt::hash(descriptor.renderPass);

    return hash;
}

bool VulkanFrameBufferCache::Functor::operator()(const VulkanFramebufferDescriptor& lhs,
                                                 const VulkanFramebufferDescriptor& rhs) const
{
    return true;
}

VulkanFrameBufferCache::VulkanFrameBufferCache(VulkanDevice* device)
    : m_device(device)
{
    // TODO
}

VulkanFrameBuffer* VulkanFrameBufferCache::getFrameBuffer(VulkanFramebufferDescriptor descriptor)
{
    auto it = m_cache.find(descriptor);
    if (it != m_cache.end())
    {
        return (it->second).get();
    }

    auto framebuffer = std::make_unique<VulkanFrameBuffer>(m_device, descriptor);
    VulkanFrameBuffer* framebufferPtr = framebuffer.get();
    m_cache.emplace(descriptor, std::move(framebuffer));

    return framebufferPtr;
}

} // namespace vkt
