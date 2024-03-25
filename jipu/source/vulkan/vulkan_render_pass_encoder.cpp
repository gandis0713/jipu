
#include "vulkan_render_pass_encoder.h"

#include "vulkan_binding_group.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include <optional>
#include <spdlog/spdlog.h>

namespace jipu
{

namespace
{

VkImageLayout generateInitialLayout(const ColorAttachment& colorAttachment)
{
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (colorAttachment.loadOp == LoadOp::kLoad)
    {
        auto& renderView = colorAttachment.resolveView.has_value() ? colorAttachment.resolveView.value().get() : colorAttachment.renderView;

        layout = downcast(renderView).getTexture()->getFinalLayout();
    }

    return layout;
}

std::vector<VkClearValue> generateClearColor(const RenderPassEncoderDescriptor& descriptor)
{
    std::vector<VkClearValue> clearValues{};

    auto addColorClearValue = [](std::vector<VkClearValue>& clearValues, const std::vector<ColorAttachment>& colorAttachments) {
        for (auto i = 0; i < colorAttachments.size(); ++i)
        {
            const auto& colorAttachment = colorAttachments[i];
            if (colorAttachment.loadOp == LoadOp::kClear)
            {
                VkClearValue colorClearValue{};
                for (uint32_t i = 0; i < 4; ++i)
                {
                    colorClearValue.color.float32[i] = colorAttachment.clearValue.float32[i];
                }
                clearValues.push_back(colorClearValue);
            }
        }
    };

    addColorClearValue(clearValues, descriptor.colorAttachments);
    if (descriptor.sampleCount > 1)
        addColorClearValue(clearValues, descriptor.colorAttachments);

    if (descriptor.depthStencilAttachment.has_value())
    {
        auto depthStencilAttachment = descriptor.depthStencilAttachment.value();
        if (depthStencilAttachment.depthLoadOp == LoadOp::kClear || depthStencilAttachment.stencilLoadOp == LoadOp::kClear)
        {
            VkClearValue depthStencilClearValue{};
            depthStencilClearValue.depthStencil = { depthStencilAttachment.clearValue.depth,
                                                    depthStencilAttachment.clearValue.stencil };

            clearValues.push_back(depthStencilClearValue);
        }
    }

    return clearValues;
}

} // namespace

VulkanRenderPassDescriptor generateVulkanRenderPassDescriptor(const RenderPassEncoderDescriptor& descriptor)
{
    if (descriptor.colorAttachments.empty())
        throw std::runtime_error("Failed to create vulkan render pass encoder due to empty color attachment.");

    VulkanRenderPassDescriptor vkdescriptor{};

    for (const auto& colorAttachment : descriptor.colorAttachments)
    {
        const auto texture = downcast(colorAttachment.renderView).getTexture();

        VkAttachmentDescription attachment{};
        attachment.format = ToVkFormat(texture->getFormat());
        attachment.loadOp = ToVkAttachmentLoadOp(colorAttachment.loadOp);
        attachment.storeOp = ToVkAttachmentStoreOp(colorAttachment.storeOp);
        attachment.samples = ToVkSampleCountFlagBits(descriptor.sampleCount);
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = generateInitialLayout(colorAttachment);
        attachment.finalLayout = downcast(texture)->getFinalLayout();

        vkdescriptor.attachmentDescriptions.push_back(attachment);
    }

    if (descriptor.sampleCount > 1)
    {
        for (auto colorAttachment : descriptor.colorAttachments)
        {
            const auto texture = downcast(colorAttachment.renderView).getTexture();

            VkAttachmentDescription attachment{};
            attachment.format = ToVkFormat(texture->getFormat());
            attachment.loadOp = ToVkAttachmentLoadOp(colorAttachment.loadOp);
            attachment.storeOp = ToVkAttachmentStoreOp(colorAttachment.storeOp);
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.samples = VK_SAMPLE_COUNT_1_BIT; // should use VK_SAMPLE_COUNT_1_BIT for resolve attachment.
            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            vkdescriptor.attachmentDescriptions.push_back(attachment);
        }
    }

    if (descriptor.depthStencilAttachment.has_value())
    {
        auto depthStencilAttachment = descriptor.depthStencilAttachment.value();

        const auto texture = downcast(depthStencilAttachment.textureView).getTexture();

        VkAttachmentDescription attachment{};
        attachment.format = ToVkFormat(texture->getFormat());
        attachment.loadOp = ToVkAttachmentLoadOp(depthStencilAttachment.depthLoadOp);
        attachment.storeOp = ToVkAttachmentStoreOp(depthStencilAttachment.depthStoreOp);
        attachment.stencilLoadOp = ToVkAttachmentLoadOp(depthStencilAttachment.stencilLoadOp);
        attachment.stencilStoreOp = ToVkAttachmentStoreOp(depthStencilAttachment.stencilStoreOp);
        attachment.samples = ToVkSampleCountFlagBits(descriptor.sampleCount);
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        vkdescriptor.attachmentDescriptions.push_back(attachment);
    }

    {
        VulkanSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        // color attachments
        uint32_t colorAttachmentCount = static_cast<uint32_t>(descriptor.colorAttachments.size());
        for (auto i = 0; i < colorAttachmentCount; ++i)
        {
            // attachment references
            VkAttachmentReference colorAttachmentReference{};
            colorAttachmentReference.attachment = i;
            colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            subpassDescription.colorAttachments.push_back(colorAttachmentReference);
        }

        std::vector<VkAttachmentReference> resolveAttachmentReferences{};
        if (descriptor.sampleCount > 1)
        {
            // resolve attachments
            for (uint32_t i = colorAttachmentCount; i < colorAttachmentCount * 2; ++i)
            {
                VkAttachmentReference resolveAttachmentReference{};
                resolveAttachmentReference.attachment = i;
                resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                subpassDescription.resolveAttachments.push_back(resolveAttachmentReference);
            }
        }

        if (descriptor.depthStencilAttachment.has_value())
        {
            VkAttachmentReference depthAttachment{};
            depthAttachment.attachment = static_cast<uint32_t>(subpassDescription.colorAttachments.size() + subpassDescription.resolveAttachments.size());
            depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            subpassDescription.depthStencilAttachment = depthAttachment;
        }
        vkdescriptor.subpassDescriptions = { subpassDescription };

        VkSubpassDependency subpassDependency{};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        if (descriptor.depthStencilAttachment.has_value())
            subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        vkdescriptor.subpassDependencies = { subpassDependency };
    }

    return vkdescriptor;
}

VulkanFramebufferDescriptor generateVulkanFramebufferDescriptor(VulkanRenderPass* renderPass, const RenderPassEncoderDescriptor& descriptor)
{
    if (descriptor.colorAttachments.empty())
        throw std::runtime_error("The attachments for color is empty to create frame buffer descriptor.");

    const auto texture = downcast(descriptor.colorAttachments[0].renderView).getTexture();

    VulkanFramebufferDescriptor vkdescriptor{};
    vkdescriptor.width = texture->getWidth();
    vkdescriptor.height = texture->getHeight();
    vkdescriptor.layers = 1;
    vkdescriptor.renderPass = renderPass->getVkRenderPass();

    for (const auto attachment : descriptor.colorAttachments)
        vkdescriptor.attachments.push_back(downcast(attachment.renderView).getVkImageView());

    if (descriptor.sampleCount > 1)
    {
        for (const auto attachment : descriptor.colorAttachments)
            vkdescriptor.attachments.push_back(downcast(attachment.resolveView.value()).getVkImageView());
    }

    if (descriptor.depthStencilAttachment.has_value())
    {
        auto depthStencilAttachment = descriptor.depthStencilAttachment.value();
        vkdescriptor.attachments.push_back(downcast(depthStencilAttachment.textureView).getVkImageView());
    }

    return vkdescriptor;
}

VulkanRenderPassEncoderDescriptor generateVulkanRenderPassEncoderDescriptor(VulkanDevice* device, const RenderPassEncoderDescriptor& descriptor)
{
    auto renderPass = device->getRenderPass(generateVulkanRenderPassDescriptor(descriptor));
    auto framebuffer = device->getFrameBuffer(generateVulkanFramebufferDescriptor(renderPass, descriptor));

    VulkanRenderPassEncoderDescriptor vkdescriptor{};
    vkdescriptor.clearValues = generateClearColor(descriptor);
    vkdescriptor.renderPass = renderPass->getVkRenderPass();
    vkdescriptor.framebuffer = framebuffer->getVkFrameBuffer();
    vkdescriptor.renderArea.offset = { 0, 0 };
    vkdescriptor.renderArea.extent = { framebuffer->getWidth(), framebuffer->getHeight() };

    return vkdescriptor;
}

VulkanRenderPassEncoder::VulkanRenderPassEncoder(VulkanCommandBuffer& commandBuffer, const RenderPassEncoderDescriptor& descriptor)
    : VulkanRenderPassEncoder(commandBuffer, generateVulkanRenderPassEncoderDescriptor(commandBuffer.getDevice(), descriptor))
{
}

VulkanRenderPassEncoder::VulkanRenderPassEncoder(VulkanCommandBuffer& commandBuffer, const VulkanRenderPassEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
    initialize();
}

void VulkanRenderPassEncoder::setPipeline(RenderPipeline& pipeline)
{
    m_pipeline = std::make_optional<VulkanRenderPipeline::Ref>(downcast(pipeline));

    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    vulkanDevice->vkAPI.CmdBindPipeline(vulkanCommandBuffer.getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.value().get().getVkPipeline());
}

void VulkanRenderPassEncoder::setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset)
{
    if (!m_pipeline.has_value())
        throw std::runtime_error("The pipeline is null opt");

    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    auto& vulkanPipelineLayout = downcast(m_pipeline.value().get().getPipelineLayout());
    auto vulkanBindingGroup = downcast(bindingGroup);
    VkDescriptorSet set = vulkanBindingGroup->getVkDescriptorSet();
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    vkAPI.CmdBindDescriptorSets(vulkanCommandBuffer.getVkCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                vulkanPipelineLayout.getVkPipelineLayout(),
                                index,
                                1,
                                &set,
                                static_cast<uint32_t>(dynamicOffset.size()),
                                dynamicOffset.data());
}

void VulkanRenderPassEncoder::setVertexBuffer(uint32_t slot, Buffer* buffer)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    auto vulkanBuffer = downcast(buffer);
    VkBuffer vertexBuffers[] = { vulkanBuffer->getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vulkanDevice->vkAPI.CmdBindVertexBuffers(vulkanCommandBuffer.getVkCommandBuffer(), slot, 1, vertexBuffers, offsets);
}

void VulkanRenderPassEncoder::setIndexBuffer(Buffer* buffer, IndexFormat format)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    auto vulkanBuffer = downcast(buffer);
    vulkanDevice->vkAPI.CmdBindIndexBuffer(vulkanCommandBuffer.getVkCommandBuffer(), vulkanBuffer->getVkBuffer(), 0, ToVkIndexType(format));
}

void VulkanRenderPassEncoder::setViewport(float x,
                                          float y,
                                          float width,
                                          float height,
                                          float minDepth,
                                          float maxDepth)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    VkViewport viewport{ x, y, width, height, minDepth, maxDepth };
    vulkanDevice->vkAPI.CmdSetViewport(vulkanCommandBuffer.getVkCommandBuffer(), 0, 1, &viewport);
}

void VulkanRenderPassEncoder::setScissor(float x,
                                         float y,
                                         float width,
                                         float height)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    VkRect2D scissorRect{};
    scissorRect.offset.x = x;
    scissorRect.offset.y = y;
    scissorRect.extent.width = width;
    scissorRect.extent.height = height;

    vulkanDevice->vkAPI.CmdSetScissor(vulkanCommandBuffer.getVkCommandBuffer(), 0, 1, &scissorRect);
}

void VulkanRenderPassEncoder::draw(uint32_t vertexCount)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    vulkanDevice->vkAPI.CmdDraw(vulkanCommandBuffer.getVkCommandBuffer(), vertexCount, 1, 0, 0);
}

void VulkanRenderPassEncoder::drawIndexed(uint32_t indexCount,
                                          uint32_t instanceCount,
                                          uint32_t indexOffset,
                                          uint32_t vertexOffset,
                                          uint32_t firstInstance)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    vulkanDevice->vkAPI.CmdDrawIndexed(vulkanCommandBuffer.getVkCommandBuffer(),
                                       indexCount,
                                       instanceCount,
                                       indexOffset,
                                       vertexOffset,
                                       firstInstance);
}

void VulkanRenderPassEncoder::end()
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    vulkanDevice->vkAPI.CmdEndRenderPass(vulkanCommandBuffer.getVkCommandBuffer());

    // TODO: generate stage from binding group.
    VkPipelineStageFlags flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    vulkanCommandBuffer.setSignalPipelineStage(flags);
}

void VulkanRenderPassEncoder::nextPass()
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    vulkanDevice->vkAPI.CmdNextSubpass(vulkanCommandBuffer.getVkCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
    ++m_passIndex;
}

void VulkanRenderPassEncoder::initialize()
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_descriptor.renderPass;
    renderPassInfo.framebuffer = m_descriptor.framebuffer;
    renderPassInfo.renderArea = m_descriptor.renderArea;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(m_descriptor.clearValues.size());
    renderPassInfo.pClearValues = m_descriptor.clearValues.data();

    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    vulkanDevice->vkAPI.CmdBeginRenderPass(vulkanCommandBuffer.getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

// Convert Helper
VkIndexType ToVkIndexType(IndexFormat format)
{
    VkIndexType type = VK_INDEX_TYPE_UINT16;
    switch (format)
    {
    case IndexFormat::kUint16:
    default:
        type = VK_INDEX_TYPE_UINT16;
        break;
    case IndexFormat::kUint32:
        type = VK_INDEX_TYPE_UINT32;
        break;
    }

    return type;
}

} // namespace jipu
