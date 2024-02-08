#include "vulkan_framebuffer.h"

#include "vulkan_device.h"
#include "vulkan_render_pass.h"
#include "vulkan_texture_view.h"

#include "utils/hash.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, const VulkanFramebufferDescriptor& descriptor)
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

VulkanFramebuffer::~VulkanFramebuffer()
{
    m_device->vkAPI.DestroyFramebuffer(m_device->getVkDevice(), m_framebuffer, nullptr);
}

VkFramebuffer VulkanFramebuffer::getVkFrameBuffer() const
{
    return m_framebuffer;
}

uint32_t VulkanFramebuffer::getWidth() const
{
    return m_width;
}

uint32_t VulkanFramebuffer::getHeight() const
{
    return m_height;
}

size_t VulkanFramebufferCache::Functor::operator()(const VulkanFramebufferDescriptor& descriptor) const
{
    size_t hash = jipu::hash(reinterpret_cast<uint64_t>(descriptor.renderPass));

    for (const auto& textureView : descriptor.textureViews)
    {
        combineHash(hash, reinterpret_cast<uint64_t>(textureView));
    }

    return hash;
}

bool VulkanFramebufferCache::Functor::operator()(const VulkanFramebufferDescriptor& lhs,
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

VulkanFramebufferCache::VulkanFramebufferCache(VulkanDevice* device)
    : m_device(device)
{
    // TODO
}

VulkanFramebuffer* VulkanFramebufferCache::getFrameBuffer(const VulkanFramebufferDescriptor& descriptor)
{
    auto it = m_cache.find(descriptor);
    if (it != m_cache.end())
    {
        return (it->second).get();
    }

    auto framebuffer = std::make_unique<VulkanFramebuffer>(m_device, descriptor);

    // get raw pointer before moving.
    VulkanFramebuffer* framebufferPtr = framebuffer.get();
    m_cache.emplace(descriptor, std::move(framebuffer));

    return framebufferPtr;
}

void VulkanFramebufferCache::clear()
{
    m_cache.clear();
}

} // namespace jipu
