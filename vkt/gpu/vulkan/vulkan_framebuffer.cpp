#include "vulkan_framebuffer.h"

#include "utils/hash.h"
#include "vulkan_device.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, const VulkanFramebufferDescriptor& descriptor)
    : m_device(device)
    , m_width(descriptor.width)
    , m_height(descriptor.height)
{
    VkFramebufferCreateInfo framebufferCreateInfo{ .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                                   .renderPass = descriptor.renderPass,
                                                   .attachmentCount = static_cast<uint32_t>(descriptor.imageViews.size()),
                                                   .pAttachments = descriptor.imageViews.data(),
                                                   .width = m_width,
                                                   .height = m_height,
                                                   .layers = 1 };

    if (m_device->vkAPI.CreateFramebuffer(device->getVkDevice(), &framebufferCreateInfo, nullptr, &m_framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
    m_device->vkAPI.DestroyFramebuffer(m_device->getVkDevice(), m_framebuffer, nullptr);
}

VkFramebuffer VulkanFrameBuffer::getVkFrameBuffer() const
{
    return m_framebuffer;
}

uint32_t VulkanFrameBuffer::getWidth() const
{
    return m_width;
}

uint32_t VulkanFrameBuffer::getHeight() const
{
    return m_height;
}

size_t VulkanFrameBufferCache::Functor::operator()(const VulkanFramebufferDescriptor& descriptor) const
{
    size_t hash = vkt::hash(descriptor.renderPass);

    combineHash(hash, descriptor.imageViews.size());
    combineHash(hash, descriptor.width);
    combineHash(hash, descriptor.height);

    return hash;
}

bool VulkanFrameBufferCache::Functor::operator()(const VulkanFramebufferDescriptor& lhs,
                                                 const VulkanFramebufferDescriptor& rhs) const
{
    if (lhs.height == rhs.height &&
        lhs.width == rhs.width &&
        lhs.renderPass == rhs.renderPass &&
        lhs.imageViews.size() == rhs.imageViews.size())
    {
        for (uint32_t index = 0; index < lhs.imageViews.size(); ++index)
        {
            if (lhs.imageViews[index] != rhs.imageViews[index])
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

VulkanFrameBufferCache::VulkanFrameBufferCache(VulkanDevice* device)
    : m_device(device)
{
    // TODO
}

VulkanFrameBuffer* VulkanFrameBufferCache::getFrameBuffer(const VulkanFramebufferDescriptor& descriptor)
{
    auto it = m_cache.find(descriptor);
    if (it != m_cache.end())
    {
        return (it->second).get();
    }

    auto framebuffer = std::make_unique<VulkanFrameBuffer>(m_device, descriptor);

    // get raw pointer before moving.
    VulkanFrameBuffer* framebufferPtr = framebuffer.get();
    m_cache.emplace(descriptor, std::move(framebuffer));

    return framebufferPtr;
}

void VulkanFrameBufferCache::clear()
{
    m_cache.clear();
}

} // namespace vkt
