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
    auto multiSamples = descriptor.samples > 1;
    auto depthStencil = descriptor.depthStencilFormat.has_value();

    uint64_t attachmentCount = 1;
    attachmentCount += depthStencil ? 1 : 0;
    attachmentCount += multiSamples ? 1 : 0;

    std::vector<VkAttachmentDescription> attachments{};
    attachments.resize(attachmentCount);

    auto attachmentIndex = 0u;
    // attachment descriptions
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = descriptor.colorFormat;
    colorAttachment.samples = descriptor.samples;
    colorAttachment.loadOp = descriptor.loadOp;
    colorAttachment.storeOp = descriptor.storeOp;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (!multiSamples)
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // If sample count is 1, attachment is used to present.
    else
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[attachmentIndex++] = colorAttachment;

    if (depthStencil)
    {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = descriptor.samples;
        depthAttachment.loadOp = descriptor.loadOp;
        depthAttachment.storeOp = descriptor.storeOp;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[attachmentIndex++] = depthAttachment;
    }

    if (multiSamples)
    {
        VkAttachmentDescription presentColorAttachment{};
        presentColorAttachment.format = descriptor.colorFormat;
        presentColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        presentColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        presentColorAttachment.storeOp = descriptor.storeOp;
        presentColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        presentColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        presentColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        presentColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachments[attachmentIndex++] = presentColorAttachment;
    }

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // attachment references
    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;

    if (depthStencil)
    {
        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
    }

    if (multiSamples)
    {
        VkAttachmentReference presentcColorAttachmentReference{};
        presentcColorAttachmentReference.attachment = 2;
        presentcColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        subpassDescription.pResolveAttachments = &presentcColorAttachmentReference;
    }

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    if (depthStencil)
        subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
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
    size_t hash = vkt::hash(descriptor.colorFormat);

    if (descriptor.depthStencilFormat.has_value())
        combineHash(hash, descriptor.depthStencilFormat.value());

    combineHash(hash, descriptor.samples);
    combineHash(hash, descriptor.loadOp);
    combineHash(hash, descriptor.storeOp);

    return hash;
}

bool VulkanRenderPassCache::Functor::operator()(const VulkanRenderPassDescriptor& lhs,
                                                const VulkanRenderPassDescriptor& rhs) const
{
    if (lhs.colorFormat == rhs.colorFormat &&
        lhs.samples == rhs.samples &&
        lhs.loadOp == rhs.loadOp &&
        lhs.storeOp == rhs.storeOp &&
        lhs.depthStencilFormat.has_value() == rhs.depthStencilFormat.has_value())
    {
        if (lhs.depthStencilFormat.has_value() && rhs.depthStencilFormat.has_value())
            return lhs.depthStencilFormat.value() == rhs.depthStencilFormat.value();

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
