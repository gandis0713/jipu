#include "vulkan_command_encoder.h"
#include "vulkan_binding_group.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

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
    const ColorAttachment& colorAttachment = m_descriptor.colorAttachments[0];
    VulkanRenderPassDescriptor renderPassDescriptor{ .format = ToVkFormat(colorAttachment.textureView->getFormat()),
                                                     .loadOp = ToVkAttachmentLoadOp(colorAttachment.loadOp),
                                                     .storeOp = ToVkAttachmentStoreOp(colorAttachment.storeOp),
                                                     .samples = VK_SAMPLE_COUNT_1_BIT /* TODO: use not vulkan defines */ };
    auto vulkanRenderPass = vulkanDevice->getRenderPass(renderPassDescriptor);

    auto vulkanTextureView = downcast(colorAttachment.textureView);
    VulkanFramebufferDescriptor framebufferDescriptor{ .renderPass = vulkanRenderPass->getVkRenderPass(),
                                                       .imageViews = { vulkanTextureView->getVkImageView() },
                                                       .width = vulkanTextureView->getWidth(),
                                                       .height = vulkanTextureView->getHeight() };
    auto vulkanFrameBuffer = vulkanDevice->getFrameBuffer(framebufferDescriptor);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->getVkRenderPass();
    renderPassInfo.framebuffer = vulkanFrameBuffer->getVkFrameBuffer();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = { vulkanFrameBuffer->getWidth(), vulkanFrameBuffer->getHeight() };

    ColorClearValue value = colorAttachment.clearValue;
    VkClearValue clearValue{};
    for (uint32_t i = 0; i < 4; ++i)
    {
        clearValue.color.float32[i] = colorAttachment.clearValue.float32[i];
    }
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;

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
    vulkanTexture->setLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandBuffer);

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

    // layout transition to new layout
    vulkanTexture->setLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandBuffer);
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