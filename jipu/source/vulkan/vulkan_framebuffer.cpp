#include "vulkan_framebuffer.h"

#include "vulkan_device.h"
#include "vulkan_render_pass.h"
#include "vulkan_texture_view.h"

#include "utils/hash.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice& device, const VulkanFramebufferDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
    VkFramebufferCreateInfo framebufferCreateInfo{ .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                                   .pNext = descriptor.next,
                                                   .flags = descriptor.flags,
                                                   .renderPass = descriptor.renderPass,
                                                   .attachmentCount = static_cast<uint32_t>(descriptor.attachments.size()),
                                                   .pAttachments = descriptor.attachments.data(),
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

VkFramebuffer VulkanFramebuffer::getVkFrameBuffer() const
{
    return m_framebuffer;
}

uint32_t VulkanFramebuffer::getWidth() const
{
    return m_descriptor.width;
}

uint32_t VulkanFramebuffer::getHeight() const
{
    return m_descriptor.height;
}

size_t VulkanFramebufferCache::Functor::operator()(const VulkanFramebufferDescriptor& descriptor) const
{
    size_t hash = jipu::hash(reinterpret_cast<uint64_t>(descriptor.renderPass));

    combineHash(hash, descriptor.flags);
    combineHash(hash, descriptor.width);
    combineHash(hash, descriptor.height);
    combineHash(hash, descriptor.layers);

    for (const auto& attachment : descriptor.attachments)
    {
        combineHash(hash, reinterpret_cast<uint64_t>(attachment));
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
        lhs.attachments.size() == rhs.attachments.size())
    {
        for (auto i = 0; i < lhs.attachments.size(); ++i)
        {
            if (lhs.attachments[i] != rhs.attachments[i])
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

VulkanFramebuffer& VulkanFramebufferCache::getFrameBuffer(const VulkanFramebufferDescriptor& descriptor)
{
    auto it = m_cache.find(descriptor);
    if (it != m_cache.end())
    {
        return *(it->second);
    }

    auto framebuffer = std::make_unique<VulkanFramebuffer>(m_device, descriptor);

    // get raw pointer before moving.
    VulkanFramebuffer* framebufferPtr = framebuffer.get();
    m_cache.emplace(descriptor, std::move(framebuffer));

    return *framebufferPtr;
}

void VulkanFramebufferCache::clear()
{
    m_cache.clear();
}

} // namespace jipu
