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

namespace jipu
{

VulkanCommandEncoder::VulkanCommandEncoder(VulkanCommandBuffer& commandBuffer, const CommandEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = ToVkCommandBufferUsageFlagBits(vulkanCommandBuffer.getUsage());
    commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    if (vulkanDevice->vkAPI.BeginCommandBuffer(vulkanCommandBuffer.getVkCommandBuffer(), &commandBufferBeginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin command buffer.");
    }
}

std::unique_ptr<ComputePassEncoder> VulkanCommandEncoder::beginComputePass(const ComputePassEncoderDescriptor& descriptor)
{
    return std::make_unique<VulkanComputePassEncoder>(downcast(m_commandBuffer), descriptor);
}

std::unique_ptr<RenderPassEncoder> VulkanCommandEncoder::beginRenderPass(const RenderPassEncoderDescriptor& descriptor)
{
    return std::make_unique<VulkanRenderPassEncoder>(downcast(m_commandBuffer), descriptor);
}

std::unique_ptr<RenderPassEncoder> VulkanCommandEncoder::beginRenderPass(const VulkanRenderPassEncoderDescriptor& descriptor)
{
    return std::make_unique<VulkanRenderPassEncoder>(downcast(m_commandBuffer), descriptor);
}

void VulkanCommandEncoder::copyBufferToBuffer(const BlitBuffer& src, const BlitBuffer& dst, uint64_t size)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;

    VkBuffer srcBuffer = downcast(src.buffer).getVkBuffer();
    VkBuffer dstBuffer = downcast(dst.buffer).getVkBuffer();

    vkAPI.CmdCopyBuffer(vulkanCommandBuffer.getVkCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);
}

void VulkanCommandEncoder::copyBufferToTexture(const BlitTextureBuffer& textureBuffer, const BlitTexture& texture, const Extent3D& extent)
{
    auto& vulkanTexture = downcast(texture.texture);
    if (!(vulkanTexture.getUsage() & TextureUsageFlagBits::kCopyDst))
        throw std::runtime_error("The texture is not used for copy dst.");

    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    VkCommandBuffer commandBuffer = vulkanCommandBuffer.getVkCommandBuffer();
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkImageSubresourceRange range;
    range.aspectMask = ToVkImageAspectFlags(texture.aspect);
    range.baseMipLevel = 0;
    range.levelCount = texture.texture.getMipLevels();
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    // set pipeline barrier for dst
    vulkanTexture.setPipelineBarrier(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

    // copy buffer to texture
    auto& vulkanBuffer = downcast(textureBuffer.buffer);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = ToVkImageAspectFlags(texture.aspect);
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { .width = extent.width,
                           .height = extent.height,
                           .depth = extent.depth };

    vkAPI.CmdCopyBufferToImage(commandBuffer,
                               vulkanBuffer.getVkBuffer(),
                               vulkanTexture.getVkImage(),
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

    if (uint32_t mipLevels = texture.texture.getMipLevels(); mipLevels > 1)
    {
        VkImage image = vulkanTexture.getVkImage();

        int32_t width = static_cast<int32_t>(texture.texture.getWidth());
        int32_t height = static_cast<int32_t>(texture.texture.getHeight());
        for (uint32_t i = 1; i < mipLevels; ++i)
        {

            VkImageSubresourceRange srcSubresourceRange{};
            srcSubresourceRange.aspectMask = ToVkImageAspectFlags(texture.aspect);
            srcSubresourceRange.baseMipLevel = i - 1;
            srcSubresourceRange.levelCount = 1;
            srcSubresourceRange.baseArrayLayer = 0;
            srcSubresourceRange.layerCount = 1;

            // set pipeline barrier for src
            vulkanTexture.setPipelineBarrier(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcSubresourceRange);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { width, height, 1 };
            blit.srcSubresource.aspectMask = ToVkImageAspectFlags(texture.aspect);
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { width > 1 ? width / 2 : 1, height > 1 ? height / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = ToVkImageAspectFlags(texture.aspect);
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkAPI.CmdBlitImage(commandBuffer,
                               image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1, &blit,
                               VK_FILTER_LINEAR);

            // set pipeline barrier for dst
            vulkanTexture.setPipelineBarrier(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, srcSubresourceRange);

            width = std::max(width >> 1, 1);
            height = std::max(height >> 1, 1);
        }
    }

    // set pipeline barrier to restore final layout
    vulkanTexture.setPipelineBarrier(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vulkanTexture.getFinalLayout(), range);
}

void VulkanCommandEncoder::copyTextureToBuffer(const BlitTexture& texture, const BlitTextureBuffer& buffer, const Extent3D& extent)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    // set pipeline barrier to change image layout
    auto& vulkanTexture = downcast(texture.texture);

    VkImageSubresourceRange range{};
    range.aspectMask = ToVkImageAspectFlags(texture.aspect);
    range.baseArrayLayer = 0;
    range.layerCount = 1;
    range.baseMipLevel = 0;
    range.levelCount = 1;

    auto srcImage = vulkanTexture.getVkImage();

    auto& vulkanBuffer = downcast(buffer.buffer);
    auto dstBuffer = vulkanBuffer.getVkBuffer();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = ToVkImageAspectFlags(texture.aspect);
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { .width = extent.width,
                           .height = extent.height,
                           .depth = extent.depth };

    // layout transition before copy.
    vulkanTexture.setPipelineBarrier(vulkanCommandBuffer.getVkCommandBuffer(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, range);
    vkAPI.CmdCopyImageToBuffer(vulkanCommandBuffer.getVkCommandBuffer(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer, 1, &region);

    // set pipeline barrier to restore final layout.
    vulkanTexture.setPipelineBarrier(vulkanCommandBuffer.getVkCommandBuffer(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vulkanTexture.getFinalLayout(), range);
}

void VulkanCommandEncoder::copyTextureToTexture(const BlitTexture& src, const BlitTexture& dst, const Extent3D& extent)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    // set pipeline barrier to change image layout for src
    VkImageSubresourceRange srcSubresourceRange{};
    srcSubresourceRange.aspectMask = ToVkImageAspectFlags(src.aspect);
    srcSubresourceRange.baseArrayLayer = 0;
    srcSubresourceRange.layerCount = 1;
    srcSubresourceRange.baseMipLevel = 0;
    srcSubresourceRange.levelCount = 1;

    auto& srcTexture = downcast(src.texture);
    srcTexture.setPipelineBarrier(vulkanCommandBuffer.getVkCommandBuffer(), srcTexture.getFinalLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcSubresourceRange);

    // set pipeline barrier to change image layout for dst
    VkImageSubresourceRange dstSubresourceRange{};
    dstSubresourceRange.aspectMask = ToVkImageAspectFlags(dst.aspect);
    dstSubresourceRange.baseArrayLayer = 0;
    dstSubresourceRange.layerCount = 1;
    dstSubresourceRange.baseMipLevel = 0;
    dstSubresourceRange.levelCount = 1;

    auto& dstTexture = downcast(dst.texture);
    dstTexture.setPipelineBarrier(vulkanCommandBuffer.getVkCommandBuffer(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstSubresourceRange);

    VkImageCopy copyRegion = {};
    copyRegion.srcSubresource.aspectMask = ToVkImageAspectFlags(src.aspect);
    copyRegion.srcSubresource.mipLevel = 0;
    copyRegion.srcSubresource.baseArrayLayer = 0;
    copyRegion.srcSubresource.layerCount = 1;

    copyRegion.srcOffset = { 0, 0, 0 };

    copyRegion.dstSubresource.aspectMask = ToVkImageAspectFlags(dst.aspect);
    copyRegion.dstSubresource.mipLevel = 0;
    copyRegion.dstSubresource.baseArrayLayer = 0;
    copyRegion.dstSubresource.layerCount = 1;

    copyRegion.dstOffset = { 0, 0, 0 };

    copyRegion.extent.width = extent.width;
    copyRegion.extent.height = extent.height;
    copyRegion.extent.depth = extent.depth;

    auto srcImage = srcTexture.getVkImage();
    auto dstImage = dstTexture.getVkImage();
    vkAPI.CmdCopyImage(vulkanCommandBuffer.getVkCommandBuffer(),
                       srcImage,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       dstImage,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &copyRegion);

    // set pipeline barrier to restore final layout.
    srcTexture.setPipelineBarrier(vulkanCommandBuffer.getVkCommandBuffer(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcTexture.getFinalLayout(), srcSubresourceRange);
    dstTexture.setPipelineBarrier(vulkanCommandBuffer.getVkCommandBuffer(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstTexture.getFinalLayout(), dstSubresourceRange);
}

CommandBuffer& VulkanCommandEncoder::finish()
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());

    if (vulkanDevice->vkAPI.EndCommandBuffer(vulkanCommandBuffer.getVkCommandBuffer()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to end command buffer.");
    }

    return m_commandBuffer;
}

VulkanCommandBuffer& VulkanCommandEncoder::getCommandBuffer() const
{
    return m_commandBuffer;
}

} // namespace jipu