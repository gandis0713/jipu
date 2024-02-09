#include "vulkan_render_pass.h"
#include "utils/hash.h"
#include "vulkan_device.h"
#include "vulkan_texture.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanRenderPass::VulkanRenderPass(VulkanDevice* vulkanDevice, const VulkanRenderPassDescriptor& descriptor)
    : m_device(vulkanDevice)
{
    std::vector<VkAttachmentDescription> attachments{};

    for (auto colorAttachment : descriptor.colorAttachments)
    {
        VkAttachmentDescription colorAttachmentDescription{};
        colorAttachmentDescription.format = ToVkFormat(colorAttachment.format);
        colorAttachmentDescription.loadOp = ToVkAttachmentLoadOp(colorAttachment.loadOp);
        colorAttachmentDescription.storeOp = ToVkAttachmentStoreOp(colorAttachment.storeOp);
        colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentDescription.samples = ToVkSampleCountFlagBits(descriptor.sampleCount);
        colorAttachmentDescription.initialLayout = colorAttachment.initialLayout;
        colorAttachmentDescription.finalLayout = colorAttachment.finalLayout;

        attachments.push_back(colorAttachmentDescription);
    }

    if (descriptor.sampleCount > 1)
    {
        for (auto colorAttachment : descriptor.colorAttachments)
        {
            VkAttachmentDescription attachmentDescription{};
            attachmentDescription.format = ToVkFormat(colorAttachment.format);
            attachmentDescription.loadOp = ToVkAttachmentLoadOp(colorAttachment.loadOp);
            attachmentDescription.storeOp = ToVkAttachmentStoreOp(colorAttachment.storeOp);
            attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT; // should use VK_SAMPLE_COUNT_1_BIT for resolve attachment.
            attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            attachments.push_back(attachmentDescription);
        }
    }

    if (descriptor.depthStencilAttachment.has_value())
    {
        auto depthStencilAttachment = descriptor.depthStencilAttachment.value();
        VkAttachmentDescription depthAttachmentDescription{};
        depthAttachmentDescription.format = ToVkFormat(depthStencilAttachment.format);
        depthAttachmentDescription.samples = ToVkSampleCountFlagBits(descriptor.sampleCount);
        depthAttachmentDescription.loadOp = ToVkAttachmentLoadOp(depthStencilAttachment.depthLoadOp);
        depthAttachmentDescription.storeOp = ToVkAttachmentStoreOp(depthStencilAttachment.depthStoreOp);
        depthAttachmentDescription.stencilLoadOp = ToVkAttachmentLoadOp(depthStencilAttachment.stencilLoadOp);
        depthAttachmentDescription.stencilStoreOp = ToVkAttachmentStoreOp(depthStencilAttachment.stencilStoreOp);
        depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments.push_back(depthAttachmentDescription);
    }

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // color attachments
    uint32_t colorAttachmentCount = static_cast<uint32_t>(descriptor.colorAttachments.size());
    std::vector<VkAttachmentReference> colorAttachmentReferences{};
    for (auto i = 0; i < colorAttachmentCount; ++i)
    {
        // attachment references
        VkAttachmentReference colorAttachmentReference{};
        colorAttachmentReference.attachment = i;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        colorAttachmentReferences.push_back(colorAttachmentReference);
    }
    subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
    subpassDescription.pColorAttachments = colorAttachmentReferences.data();

    std::vector<VkAttachmentReference> resolveAttachmentReferences{};
    if (descriptor.sampleCount > 1)
    {
        // resolve attachments
        for (uint32_t i = colorAttachmentCount; i < colorAttachmentCount * 2; ++i)
        {
            VkAttachmentReference resolveAttachmentReference{};
            resolveAttachmentReference.attachment = i;
            resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            resolveAttachmentReferences.push_back(resolveAttachmentReference);
        }
        subpassDescription.pResolveAttachments = resolveAttachmentReferences.data();
    }

    VkAttachmentReference depthAttachment{};
    if (descriptor.depthStencilAttachment.has_value())
    {
        depthAttachment.attachment = static_cast<uint32_t>(colorAttachmentReferences.size() + resolveAttachmentReferences.size());
        depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        subpassDescription.pDepthStencilAttachment = &depthAttachment;
    }

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    if (descriptor.depthStencilAttachment.has_value())
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

VulkanRenderPass::VulkanRenderPass(VulkanDevice* vulkanDevice, const std::vector<VulkanRenderPassDescriptor>& descriptors)
    : m_device(vulkanDevice)
{
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

size_t VulkanRenderPassCache::Functor::operator()(const std::vector<VulkanRenderPassDescriptor>& descriptors) const
{
    size_t hash = 0;

    for (const auto& descriptor : descriptors)
    {
        combineHash(hash, descriptor.sampleCount);

        for (auto colorAttachment : descriptor.colorAttachments)
        {
            combineHash(hash, colorAttachment.loadOp);
            combineHash(hash, colorAttachment.storeOp);
            combineHash(hash, colorAttachment.format);
            combineHash(hash, colorAttachment.finalLayout);
        }

        if (descriptor.depthStencilAttachment)
        {
            auto depthStencilAttachment = descriptor.depthStencilAttachment.value();

            combineHash(hash, depthStencilAttachment.depthLoadOp);
            combineHash(hash, depthStencilAttachment.depthStoreOp);
            combineHash(hash, depthStencilAttachment.stencilLoadOp);
            combineHash(hash, depthStencilAttachment.stencilStoreOp);
            combineHash(hash, depthStencilAttachment.format);
        }
    }

    return hash;
}

bool VulkanRenderPassCache::Functor::operator()(const std::vector<VulkanRenderPassDescriptor>& lhs,
                                                const std::vector<VulkanRenderPassDescriptor>& rhs) const
{
    if (lhs.size() != rhs.size())
        return false;

    for (auto i = 0; i < lhs.size(); ++i)
    {
        const auto& lhsd = lhs[i];
        const auto& rhsd = rhs[i];

        if (lhsd.colorAttachments.size() != rhsd.colorAttachments.size() ||
            lhsd.depthStencilAttachment.has_value() != rhsd.depthStencilAttachment.has_value() ||
            lhsd.sampleCount != rhsd.sampleCount)
        {
            return false;
        }

        auto colorAttanchmentSize = lhsd.colorAttachments.size();
        for (auto i = 0; i < colorAttanchmentSize; ++i)
        {
            if (lhsd.colorAttachments[i].loadOp != rhsd.colorAttachments[i].loadOp ||
                lhsd.colorAttachments[i].storeOp != rhsd.colorAttachments[i].storeOp ||
                lhsd.colorAttachments[i].format != rhsd.colorAttachments[i].format ||
                lhsd.colorAttachments[i].finalLayout != rhsd.colorAttachments[i].finalLayout)
            {
                return false;
            }
        }

        if (lhsd.depthStencilAttachment.has_value() && rhsd.depthStencilAttachment.has_value())
        {
            auto lhsDepthStencilAttachment = lhsd.depthStencilAttachment.value();
            auto rhsDepthStencilAttachment = rhsd.depthStencilAttachment.value();

            if (lhsDepthStencilAttachment.depthLoadOp != rhsDepthStencilAttachment.depthLoadOp ||
                lhsDepthStencilAttachment.depthStoreOp != rhsDepthStencilAttachment.depthStoreOp ||
                lhsDepthStencilAttachment.stencilLoadOp != rhsDepthStencilAttachment.stencilLoadOp ||
                lhsDepthStencilAttachment.stencilStoreOp != rhsDepthStencilAttachment.stencilStoreOp ||
                lhsDepthStencilAttachment.format != rhsDepthStencilAttachment.format)
            {
                return false;
            }
        }
    }

    return true;
}

VulkanRenderPassCache::VulkanRenderPassCache(VulkanDevice* device)
    : m_device(device)
{
}

VulkanRenderPass* VulkanRenderPassCache::getRenderPass(const std::vector<VulkanRenderPassDescriptor>& descriptors)
{
    auto it = m_cache.find(descriptors);
    if (it != m_cache.end())
    {
        return (it->second).get();
    }

    // log
    {
        // auto colorAttanchmentSize = descriptor.colorAttachments.size();
        // for (auto i = 0; i < colorAttanchmentSize; ++i)
        // {
        //     spdlog::trace("{} index color attachment for new renderpass.", i);
        //     spdlog::trace("  loadOp {}", static_cast<uint32_t>(descriptor.colorAttachments[i].loadOp));
        //     spdlog::trace("  storeOp {}", static_cast<uint32_t>(descriptor.colorAttachments[i].storeOp));
        //     spdlog::trace("  format {}", static_cast<uint32_t>(descriptor.colorAttachments[i].format));
        //     spdlog::trace("  finalLayout {}", static_cast<uint32_t>(descriptor.colorAttachments[i].finalLayout));
        // }

        // if (descriptor.depthStencilAttachment.has_value())
        // {
        //     auto lhsDepthStencilAttachment = descriptor.depthStencilAttachment.value();
        //     spdlog::trace("depth/stencil attachment for new renderpass.");
        //     spdlog::trace("  depthLoadOp {}", static_cast<uint32_t>(lhsDepthStencilAttachment.depthLoadOp));
        //     spdlog::trace("  depthStoreOp {}", static_cast<uint32_t>(lhsDepthStencilAttachment.depthStoreOp));
        //     spdlog::trace("  stencilLoadOp {}", static_cast<uint32_t>(lhsDepthStencilAttachment.stencilLoadOp));
        //     spdlog::trace("  stencilStoreOp {}", static_cast<uint32_t>(lhsDepthStencilAttachment.stencilStoreOp));
        //     spdlog::trace("  format {}", static_cast<uint32_t>(lhsDepthStencilAttachment.format));
        // }
    }

    // create new renderpass
    std::unique_ptr<VulkanRenderPass> renderPass = descriptors.size() == 1
                                                       ? std::make_unique<VulkanRenderPass>(m_device, descriptors[0])
                                                       : std::make_unique<VulkanRenderPass>(m_device, descriptors);

    // get raw pointer before moving.
    VulkanRenderPass* renderPassPtr = renderPass.get();
    auto result = m_cache.emplace(descriptors, std::move(renderPass));

    return renderPassPtr;
}

void VulkanRenderPassCache::clear()
{
    m_cache.clear();
}

// Convert Helper
VkAttachmentLoadOp ToVkAttachmentLoadOp(LoadOp loadOp)
{
    switch (loadOp)
    {
    case LoadOp::kClear:
        return VK_ATTACHMENT_LOAD_OP_CLEAR;

    case LoadOp::kLoad:
        return VK_ATTACHMENT_LOAD_OP_LOAD;

    case LoadOp::kDontCare:
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;

    default:
        spdlog::error("{} Load Op type is not supported.", static_cast<uint8_t>(loadOp));
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

LoadOp ToVkAttachmentLoadOp(VkAttachmentLoadOp loadOp)
{
    switch (loadOp)
    {
    case VK_ATTACHMENT_LOAD_OP_CLEAR:
        return LoadOp::kClear;

    case VK_ATTACHMENT_LOAD_OP_LOAD:
        return LoadOp::kLoad;

    case VK_ATTACHMENT_LOAD_OP_DONT_CARE:
        return LoadOp::kDontCare;

    default:
        spdlog::error("{} Load Op type is not supported.", static_cast<int32_t>(loadOp));
        return LoadOp::kDontCare;
    }
}

VkAttachmentStoreOp ToVkAttachmentStoreOp(StoreOp storeOp)
{
    switch (storeOp)
    {
    case StoreOp::kStore:
        return VK_ATTACHMENT_STORE_OP_STORE;

    case StoreOp::kDontCare:
        return VK_ATTACHMENT_STORE_OP_DONT_CARE;

    default:
        spdlog::error("{} Store Op type is not supported.", static_cast<uint8_t>(storeOp));
        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
}

StoreOp ToStoreOp(VkAttachmentStoreOp storeOp)
{
    switch (storeOp)
    {
    case VK_ATTACHMENT_STORE_OP_STORE:
        return StoreOp::kStore;

    case VK_ATTACHMENT_STORE_OP_DONT_CARE:
        return StoreOp::kDontCare;

    default:
        spdlog::error("{} Store Op type is not supported.", static_cast<uint8_t>(storeOp));
        return StoreOp::kDontCare;
    }
}

} // namespace jipu
