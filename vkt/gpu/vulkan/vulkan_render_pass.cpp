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
    std::vector<VkAttachmentDescription> attachments{};
    attachments.resize(2); // set only for color and depth/stencil attachment.

    // attachment descriptions
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = descriptor.format;
    colorAttachment.samples = descriptor.samples;
    colorAttachment.loadOp = descriptor.loadOp;
    colorAttachment.storeOp = descriptor.storeOp;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (descriptor.samples == VK_SAMPLE_COUNT_1_BIT)
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // If sample count is 1, attachment is used to present.
    else
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0] = colorAttachment;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = descriptor.samples;
    depthAttachment.loadOp = descriptor.loadOp;
    depthAttachment.storeOp = descriptor.storeOp;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1] = depthAttachment;

    if (descriptor.samples != VK_SAMPLE_COUNT_1_BIT)
    {
        VkAttachmentDescription presentColorAttachment{};
        presentColorAttachment.format = descriptor.format;
        presentColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        presentColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        presentColorAttachment.storeOp = descriptor.storeOp;
        presentColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        presentColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        presentColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        presentColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachments.push_back(presentColorAttachment);
    }

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // attachment references
    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;

    if (descriptor.samples != VK_SAMPLE_COUNT_1_BIT)
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
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

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
