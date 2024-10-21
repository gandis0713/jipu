#include "vulkan_framebuffer.h"

#include "vulkan_device.h"
#include "vulkan_render_pass.h"
#include "vulkan_texture_view.h"

#include "common/hash.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice& device, const VulkanFramebufferDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
    std::vector<VkImageView> attachments{};
    for (const auto attachment : descriptor.colorAttachments)
    {
        attachments.push_back(attachment.renderView->getVkImageView());
        if (attachment.resolveView)
            attachments.push_back(attachment.resolveView->getVkImageView());
    }

    if (descriptor.depthStencilAttachment)
    {
        attachments.push_back(descriptor.depthStencilAttachment->getVkImageView());
    }

    VkFramebufferCreateInfo framebufferCreateInfo{ .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                                   .pNext = descriptor.next,
                                                   .flags = descriptor.flags,
                                                   .renderPass = descriptor.renderPass->getVkRenderPass(),
                                                   .attachmentCount = static_cast<uint32_t>(attachments.size()),
                                                   .pAttachments = attachments.data(),
                                                   .width = descriptor.width,
                                                   .height = descriptor.height,
                                                   .layers = descriptor.layers };

    if (m_device.vkAPI.CreateFramebuffer(device.getVkDevice(), &framebufferCreateInfo, nullptr, &m_framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create framebuffer!");
    }
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    m_device.vkAPI.DestroyFramebuffer(m_device.getVkDevice(), m_framebuffer, nullptr);
}

const std::vector<FramebufferColorAttachment>& VulkanFramebuffer::getColorAttachments() const
{
    return m_descriptor.colorAttachments;
}

uint32_t VulkanFramebuffer::getWidth() const
{
    return m_descriptor.width;
}

uint32_t VulkanFramebuffer::getHeight() const
{
    return m_descriptor.height;
}

VkFramebuffer VulkanFramebuffer::getVkFrameBuffer() const
{
    return m_framebuffer;
}

size_t VulkanFramebufferCache::Functor::operator()(const VulkanFramebufferDescriptor& descriptor) const
{
    size_t hash = jipu::hash(reinterpret_cast<uint64_t>(descriptor.renderPass));

    combineHash(hash, descriptor.flags);
    combineHash(hash, descriptor.width);
    combineHash(hash, descriptor.height);
    combineHash(hash, descriptor.layers);

    for (const auto& attachment : descriptor.colorAttachments)
    {
        combineHash(hash, reinterpret_cast<uint64_t>(attachment.renderView));
        combineHash(hash, reinterpret_cast<uint64_t>(attachment.resolveView));
    }

    return hash;
}

bool VulkanFramebufferCache::Functor::operator()(const VulkanFramebufferDescriptor& lhs,
                                                 const VulkanFramebufferDescriptor& rhs) const
{
    if (lhs.flags == rhs.flags &&
        lhs.width == rhs.width &&
        lhs.height == rhs.height &&
        lhs.layers == rhs.layers &&
        lhs.renderPass == rhs.renderPass &&
        lhs.colorAttachments.size() == rhs.colorAttachments.size() &&
        lhs.depthStencilAttachment == rhs.depthStencilAttachment)
    {
        for (auto i = 0; i < lhs.colorAttachments.size(); ++i)
        {
            if (lhs.colorAttachments[i].renderView != rhs.colorAttachments[i].renderView)
                return false;

            if (lhs.colorAttachments[i].resolveView != rhs.colorAttachments[i].resolveView)
                return false;
        }

        return true;
    }

    return false;
}

VulkanFramebufferCache::VulkanFramebufferCache(VulkanDevice& device)
    : m_device(device)
{
    // TODO
}

VulkanFramebuffer* VulkanFramebufferCache::getFrameBuffer(const VulkanFramebufferDescriptor& descriptor)
{
    auto it = m_cache.find(descriptor);
    if (it != m_cache.end())
    {
        return it->second.get();
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
