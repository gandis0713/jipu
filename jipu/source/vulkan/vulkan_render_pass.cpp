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
    initialize({ descriptor });
}

VulkanRenderPass::VulkanRenderPass(VulkanDevice* vulkanDevice, const std::vector<VulkanRenderPassDescriptor>& descriptors)
    : m_device(vulkanDevice)
{
    initialize(descriptors);
}

VulkanRenderPass::~VulkanRenderPass()
{
    auto vulkanDevice = downcast(m_device);

    vulkanDevice->vkAPI.DestroyRenderPass(vulkanDevice->getVkDevice(), m_renderPass, nullptr);
}

void VulkanRenderPass::initialize(const std::vector<VulkanRenderPassDescriptor>& descriptors)
{
    auto vulkanDevice = downcast(m_device);

    std::vector<VkAttachmentDescription> attachmentDescriptions{};
    for (auto i = 0; i < descriptors.size(); ++i)
    {
        const auto& descriptor = descriptors[i];

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

            attachmentDescriptions.push_back(colorAttachmentDescription);
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

                attachmentDescriptions.push_back(attachmentDescription);
            }
        }
    }

    for (auto i = 0; i < descriptors.size(); ++i) // TODO: fix me
    {
        const auto& descriptor = descriptors[i];

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
            attachmentDescriptions.push_back(depthAttachmentDescription);

            spdlog::debug("create depth render pass in render pass.");
            break;
        }
    }

    std::vector<VkSubpassDescription> subpassDescriptions{};
    subpassDescriptions.resize(descriptors.size());

    // hard coding for test.

    // first pass
    // color attachments
    std::vector<VkAttachmentReference> firstPassColorAttachmentReferences{};
    firstPassColorAttachmentReferences.resize(3);
    VkAttachmentReference firstPassDepthAttachment{ 4, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    {
        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        // color attachments
        firstPassColorAttachmentReferences[0] = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        firstPassColorAttachmentReferences[1] = { 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        firstPassColorAttachmentReferences[2] = { 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

        // if (descriptors[0].sampleCount > 1)
        // {
        //     std::vector<VkAttachmentReference> resolveAttachmentReferences{};
        //     resolveAttachmentReferences.resize(3);
        //     resolveAttachmentReferences[0] = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        //     resolveAttachmentReferences[1] = { 1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        //     resolveAttachmentReferences[2] = { 2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

        //     subpassDescription.pResolveAttachments = resolveAttachmentReferences.data();
        // }

        subpassDescription.colorAttachmentCount = static_cast<uint32_t>(firstPassColorAttachmentReferences.size());
        subpassDescription.pColorAttachments = firstPassColorAttachmentReferences.data();
        subpassDescription.pDepthStencilAttachment = &firstPassDepthAttachment;

        subpassDescriptions[0] = subpassDescription;
    }

    // second pass
    // color attachments
    std::vector<VkAttachmentReference> secondPassColorAttachmentReferences{};
    secondPassColorAttachmentReferences.resize(1);
    VkAttachmentReference secondPassDepthAttachment{ 4, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    std::vector<VkAttachmentReference> secondPassInputAttachmentReferences{};
    secondPassInputAttachmentReferences.resize(3);
    {
        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        // color attachments
        secondPassColorAttachmentReferences[0] = { 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

        // if (descriptor.sampleCount > 1) // fix me
        // {
        //     std::vector<VkAttachmentReference> resolveAttachmentReferences{};
        //     resolveAttachmentReferences.resize(1);
        //     resolveAttachmentReferences[0] = { 3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

        //     subpassDescription.pResolveAttachments = resolveAttachmentReferences.data();
        // }

        VkAttachmentReference depthAttachment{ 4, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

        secondPassInputAttachmentReferences[0] = { 0, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
        secondPassInputAttachmentReferences[1] = { 1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
        secondPassInputAttachmentReferences[2] = { 2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

        subpassDescription.colorAttachmentCount = static_cast<uint32_t>(secondPassColorAttachmentReferences.size());
        subpassDescription.pColorAttachments = secondPassColorAttachmentReferences.data();
        subpassDescription.pDepthStencilAttachment = &secondPassDepthAttachment;
        subpassDescription.inputAttachmentCount = static_cast<uint32_t>(secondPassInputAttachmentReferences.size());
        subpassDescription.pInputAttachments = secondPassInputAttachmentReferences.data();

        subpassDescriptions[1] = subpassDescription;
    }

    std::vector<VkSubpassDependency> subpassDependencies{};
    subpassDependencies.resize(3);

    // This makes sure that writes to the depth image are done before we try to write to it again
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    subpassDependencies[0].srcAccessMask = 0;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = 0;

    subpassDependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].dstSubpass = 0;
    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].srcAccessMask = 0;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[1].dependencyFlags = 0;

    subpassDependencies[2].srcSubpass = 0;
    subpassDependencies[2].dstSubpass = 1;
    subpassDependencies[2].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[2].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    subpassDependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[2].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    subpassDependencies[2].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    if (vulkanDevice->vkAPI.CreateRenderPass(vulkanDevice->getVkDevice(), &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
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
