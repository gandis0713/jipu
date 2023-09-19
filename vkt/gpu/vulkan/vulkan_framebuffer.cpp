#include "vulkan_framebuffer.h"

#include "vulkan_device.h"
#include "vulkan_render_pass.h"
#include "vulkan_texture_view.h"

#include "utils/hash.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, const VulkanFramebufferDescriptor& descriptor)
    : m_device(device)
{
    if (descriptor.textureViews.empty())
        throw std::runtime_error("Failed to create vulkan frame buffer due to empty texture view.");

    const auto& textureView = descriptor.textureViews[0];
    m_width = textureView->getWidth();
    m_height = textureView->getHeight();

    uint32_t textureViewSize = static_cast<uint32_t>(descriptor.textureViews.size());
    std::vector<VkImageView> imageViews(textureViewSize);
    for (auto i = 0; i < textureViewSize; ++i)
    {
        imageViews[i] = downcast(descriptor.textureViews[i])->getVkImageView();
    }

    VkFramebufferCreateInfo framebufferCreateInfo{ .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                                   .renderPass = descriptor.renderPass->getVkRenderPass(),
                                                   .attachmentCount = static_cast<uint32_t>(imageViews.size()),
                                                   .pAttachments = imageViews.data(),
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
    size_t hash = vkt::hash(reinterpret_cast<uint64_t>(descriptor.renderPass));

    for (const auto& textureView : descriptor.textureViews)
    {
        combineHash(hash, reinterpret_cast<uint64_t>(textureView));
    }

    return hash;
}

bool VulkanFrameBufferCache::Functor::operator()(const VulkanFramebufferDescriptor& lhs,
                                                 const VulkanFramebufferDescriptor& rhs) const
{
    if (lhs.renderPass == rhs.renderPass &&
        lhs.textureViews.size() == rhs.textureViews.size())
    {
        for (auto i = 0; i < lhs.textureViews.size(); ++i)
        {
            if (lhs.textureViews[i] != rhs.textureViews[i])
                return false;
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
