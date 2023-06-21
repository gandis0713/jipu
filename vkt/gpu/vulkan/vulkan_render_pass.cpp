#include "vulkan_render_pass.h"
#include "utils/hash.h"
#include "vulkan_device.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

VulkanRenderPass::VulkanRenderPass(VulkanDevice* vulkanDevice, VulkanRenderPassDescriptor descriptor)
    : m_device(vulkanDevice)
{
    VkAttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.format = descriptor.format;
    colorAttachmentDescription.samples = descriptor.samples;
    colorAttachmentDescription.loadOp = descriptor.loadOp;
    colorAttachmentDescription.storeOp = descriptor.storeOp;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    if (vulkanDevice->vkAPI.CreateRenderPass(vulkanDevice->getVkDevice(), &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

VulkanRenderPass::~VulkanRenderPass()
{
    auto vulkanDevice = downcast(m_device);

    vulkanDevice->vkAPI.DestroyRenderPass(vulkanDevice->getVkDevice(), m_renderPass, nullptr);
}

VkRenderPass VulkanRenderPass::getVkRenderPass() const
{
    return m_renderPass;
}

size_t VulkanRenderPassCache::Functor::operator()(const VulkanRenderPassDescriptor& descriptor) const
{
    size_t hash = vkt::hash(descriptor.format);

    combineHash(hash, descriptor.samples);
    combineHash(hash, descriptor.loadOp);
    combineHash(hash, descriptor.storeOp);

    return hash;
}

bool VulkanRenderPassCache::Functor::operator()(const VulkanRenderPassDescriptor& lhs,
                                                const VulkanRenderPassDescriptor& rhs) const
{
    if (lhs.format == rhs.format &&
        lhs.samples == rhs.samples &&
        lhs.loadOp == rhs.loadOp &&
        lhs.storeOp == rhs.storeOp)
    {
        return true;
    }

    return false;
}

VulkanRenderPassCache::VulkanRenderPassCache(VulkanDevice* device)
    : m_device(device)
{
}

VulkanRenderPass* VulkanRenderPassCache::getRenderPass(const VulkanRenderPassDescriptor& descriptor)
{
    auto it = m_cache.find(descriptor);
    if (it != m_cache.end())
    {
        return (it->second).get();
    }

    auto renderPass = std::make_unique<VulkanRenderPass>(m_device, descriptor);

    // get raw pointer before moving.
    VulkanRenderPass* renderPassPtr = renderPass.get();
    m_cache.emplace(descriptor, std::move(renderPass));

    return renderPassPtr;
}

void VulkanRenderPassCache::clear()
{
    m_cache.clear();
}

} // namespace vkt
