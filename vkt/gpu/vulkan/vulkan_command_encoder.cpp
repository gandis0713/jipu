#include "vulkan_command_encoder.h"
#include "vulkan_binding_group.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include <algorithm>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

VulkanRenderCommandEncoder::VulkanRenderCommandEncoder(VulkanCommandBuffer* commandBuffer, const RenderCommandEncoderDescriptor& descriptor)
    : RenderCommandEncoder(commandBuffer, descriptor)
{
}

void VulkanRenderCommandEncoder::begin()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = ToVkCommandBufferUsageFlagBits(vulkanCommandBuffer->getUsage());
    commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    if (vulkanDevice->vkAPI.BeginCommandBuffer(vulkanCommandBuffer->getVkCommandBuffer(), &commandBufferBeginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin command buffer.");
    }

    // TODO: support multiple color sample.
    // TODO: add depth, stencil into RenderPassDescriptor.
    const ColorAttachment& colorAttachment = m_descriptor.colorAttachments[0];
    const DepthStencilAttachment& depthStencilAttachment = m_descriptor.depthStencilAttachment;
    VulkanRenderPassDescriptor renderPassDescriptor{ .format = ToVkFormat(colorAttachment.renderView->getFormat()),
                                                     .loadOp = ToVkAttachmentLoadOp(colorAttachment.loadOp),
                                                     .storeOp = ToVkAttachmentStoreOp(colorAttachment.storeOp),
                                                     .samples = ToVkSampleCountFlagBits(colorAttachment.renderView->getSampleCount()) };
    auto vulkanRenderPass = vulkanDevice->getRenderPass(renderPassDescriptor);

    auto vulkanRenderTextureView = downcast(colorAttachment.renderView);
    auto vulkanSwapchainTextureView = downcast(colorAttachment.resolveView);
    auto vulkanDepthStencilTextureView = downcast(depthStencilAttachment.textureView);
    VulkanFramebufferDescriptor framebufferDescriptor{ .renderPass = vulkanRenderPass->getVkRenderPass(),
                                                       .imageViews = { vulkanRenderTextureView->getVkImageView(),
                                                                       vulkanDepthStencilTextureView->getVkImageView(),
                                                                       vulkanSwapchainTextureView->getVkImageView() },
                                                       .width = vulkanSwapchainTextureView->getWidth(),
                                                       .height = vulkanSwapchainTextureView->getHeight() };
    auto vulkanFrameBuffer = vulkanDevice->getFrameBuffer(framebufferDescriptor);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->getVkRenderPass();
    renderPassInfo.framebuffer = vulkanFrameBuffer->getVkFrameBuffer();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = { vulkanFrameBuffer->getWidth(), vulkanFrameBuffer->getHeight() };

    VkClearValue colorClearValue{};
    for (uint32_t i = 0; i < 4; ++i)
    {
        colorClearValue.color.float32[i] = colorAttachment.clearValue.float32[i];
    }

    VkClearValue depthStencilValue;
    depthStencilValue.depthStencil = { depthStencilAttachment.clearValue.depth, depthStencilAttachment.clearValue.stencil };

    std::array<VkClearValue, 2> clearValues{ colorClearValue, depthStencilValue };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vulkanDevice->vkAPI.CmdBeginRenderPass(vulkanCommandBuffer->getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanRenderCommandEncoder::end()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdEndRenderPass(vulkanCommandBuffer->getVkCommandBuffer());
    if (vulkanDevice->vkAPI.EndCommandBuffer(vulkanCommandBuffer->getVkCommandBuffer()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to end command buffer.");
    }
}

void VulkanRenderCommandEncoder::setPipeline(Pipeline* pipeline)
{
    m_pipeline = pipeline;

    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanPipeline = downcast(pipeline);
    vulkanDevice->vkAPI.CmdBindPipeline(vulkanCommandBuffer->getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getVkPipeline());
}

void VulkanRenderCommandEncoder::setBindingGroup(uint32_t index, BindingGroup* bindingGroup)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    auto vulkanPipelineLayout = downcast(m_pipeline->getPipelineLayout());
    auto vulkanBindingGroup = downcast(bindingGroup);
    VkDescriptorSet set = vulkanBindingGroup->getVkDescriptorSet();
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    vkAPI.CmdBindDescriptorSets(vulkanCommandBuffer->getVkCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                vulkanPipelineLayout->getVkPipelineLayout(),
                                0,
                                1,
                                &set,
                                0,
                                nullptr);
}

void VulkanRenderCommandEncoder::setVertexBuffer(Buffer* buffer)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanBuffer = downcast(buffer);
    VkBuffer vertexBuffers[] = { vulkanBuffer->getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vulkanDevice->vkAPI.CmdBindVertexBuffers(vulkanCommandBuffer->getVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
}

void VulkanRenderCommandEncoder::setIndexBuffer(Buffer* buffer)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanBuffer = downcast(buffer);
    vulkanDevice->vkAPI.CmdBindIndexBuffer(vulkanCommandBuffer->getVkCommandBuffer(), vulkanBuffer->getVkBuffer(), 0, VK_INDEX_TYPE_UINT16);
}

void VulkanRenderCommandEncoder::draw(uint32_t vertexCount)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdDraw(vulkanCommandBuffer->getVkCommandBuffer(), vertexCount, 1, 0, 0);
}

void VulkanRenderCommandEncoder::drawIndexed(uint32_t indexCount)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdDrawIndexed(vulkanCommandBuffer->getVkCommandBuffer(), indexCount, 1, 0, 0, 0);
}

void VulkanRenderCommandEncoder::setViewport(float x,
                                             float y,
                                             float width,
                                             float height,
                                             float minDepth,
                                             float maxDepth)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    VkViewport viewport{ x, y, width, height, minDepth, maxDepth };
    vulkanDevice->vkAPI.CmdSetViewport(vulkanCommandBuffer->getVkCommandBuffer(), 0, 1, &viewport);
}

void VulkanRenderCommandEncoder::setScissor(float x,
                                            float y,
                                            float width,
                                            float height)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    VkRect2D scissorRect{};
    scissorRect.offset.x = x;
    scissorRect.offset.y = y;
    scissorRect.extent.width = width;
    scissorRect.extent.height = height;

    vulkanDevice->vkAPI.CmdSetScissor(vulkanCommandBuffer->getVkCommandBuffer(), 0, 1, &scissorRect);
}

VulkanBlitCommandEncoder::VulkanBlitCommandEncoder(VulkanCommandBuffer* commandBuffer, const BlitCommandEncoderDescriptor& descriptor)
    : BlitCommandEncoder(commandBuffer, descriptor)
{
}

void VulkanBlitCommandEncoder::begin()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = ToVkCommandBufferUsageFlagBits(vulkanCommandBuffer->getUsage());
    commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

    vkAPI.BeginCommandBuffer(vulkanCommandBuffer->getVkCommandBuffer(), &commandBufferBeginInfo);
}

void VulkanBlitCommandEncoder::end()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkCommandBuffer commandBuffer = vulkanCommandBuffer->getVkCommandBuffer();
    vkAPI.EndCommandBuffer(commandBuffer);
}

void VulkanBlitCommandEncoder::copyBufferToBuffer(const BlitBuffer& src, const BlitBuffer& dst, uint64_t size)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;
}

void VulkanBlitCommandEncoder::copyBufferToTexture(const BlitTextureBuffer& textureBuffer, const BlitTexture& texture, const Extent3D& extent)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    VkCommandBuffer commandBuffer = vulkanCommandBuffer->getVkCommandBuffer();

    // layout transition to old layout
    auto vulkanTexture = downcast(texture.texture);

    VkImageSubresourceRange range;
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = texture.texture->getMipLevels();
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    vulkanTexture->setLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

    // copy buffer to texture
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    auto vulkanBuffer = downcast(textureBuffer.buffer);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { .width = extent.width,
                           .height = extent.height,
                           .depth = extent.depth };

    vkAPI.CmdCopyBufferToImage(commandBuffer,
                               vulkanBuffer->getVkBuffer(),
                               vulkanTexture->getVkImage(),
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

    if (uint32_t mipLevels = texture.texture->getMipLevels(); mipLevels > 1)
    {
        VkImage image = vulkanTexture->getVkImage();

        int32_t width = static_cast<int32_t>(texture.texture->getWidth());
        int32_t height = static_cast<int32_t>(texture.texture->getHeight());
        for (uint32_t i = 1; i < mipLevels; ++i)
        {

            VkImageSubresourceRange srcSubresourceRange{};
            srcSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            srcSubresourceRange.baseMipLevel = i - 1;
            srcSubresourceRange.levelCount = 1;
            srcSubresourceRange.baseArrayLayer = 0;
            srcSubresourceRange.layerCount = 1;

            VkImageLayout srcNewLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            VkImageLayout srcOldLayout = vulkanTexture->getLayout();
            if (srcOldLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            {
                throw std::runtime_error("The src image layout must be TRANSFER_DST_OPTIMAL before transitioning to TRANSFER_SRC_OPTIMAL.");
            }

            // layout transition for src image.
            vulkanTexture->setLayout(commandBuffer, srcNewLayout, srcSubresourceRange);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { width, height, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { width > 1 ? width / 2 : 1, height > 1 ? height / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkAPI.CmdBlitImage(commandBuffer,
                               image, srcNewLayout,
                               image, srcOldLayout,
                               1, &blit,
                               VK_FILTER_LINEAR);

            // layout transition for src image.
            vulkanTexture->setLayout(commandBuffer, srcOldLayout, srcSubresourceRange);

            width = std::max(width >> 1, 1);
            height = std::max(height >> 1, 1);
        }
    }

    // layout transition to new layout
    vulkanTexture->setLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, range);
}

void VulkanBlitCommandEncoder::copyTextureToBuffer()
{
    // TODO: not yet implemented
}

void VulkanBlitCommandEncoder::copyTextureToTexture()
{
    // TODO: not yet implemented
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

} // namespace vkt