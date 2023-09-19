#include "vulkan_command_encoder.h"
#include "vulkan_binding_group.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_compute_pass_encoder.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_render_pass_encoder.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include <algorithm>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

VulkanCommandEncoder::VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor)
    : CommandEncoder(commandBuffer, descriptor)
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
}

std::unique_ptr<ComputePassEncoder> VulkanCommandEncoder::beginComputePass(const ComputePassDescriptor& descriptor)
{
    return std::make_unique<VulkanComputePassEncoder>(downcast(m_commandBuffer), descriptor);
}

std::unique_ptr<RenderPassEncoder> VulkanCommandEncoder::beginRenderPass(const RenderPassDescriptor& descriptor)
{
    return std::make_unique<VulkanRenderPassEncoder>(downcast(m_commandBuffer), descriptor);
}

void VulkanCommandEncoder::copyBufferToBuffer(const BlitBuffer& src, const BlitBuffer& dst, uint64_t size)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;

    VkBuffer srcBuffer = downcast(src.buffer)->getVkBuffer();
    VkBuffer dstBuffer = downcast(dst.buffer)->getVkBuffer();

    vkAPI.CmdCopyBuffer(vulkanCommandBuffer->getVkCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);
}

void VulkanCommandEncoder::copyBufferToTexture(const BlitTextureBuffer& textureBuffer, const BlitTexture& texture, const Extent3D& extent)
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

void VulkanCommandEncoder::copyTextureToBuffer()
{
    // TODO: not yet implemented
}

void VulkanCommandEncoder::copyTextureToTexture()
{
    // TODO: not yet implemented
}

CommandBuffer* VulkanCommandEncoder::finish()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    if (vulkanDevice->vkAPI.EndCommandBuffer(vulkanCommandBuffer->getVkCommandBuffer()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end command buffer.");
    }

    return m_commandBuffer;
}

} // namespace vkt