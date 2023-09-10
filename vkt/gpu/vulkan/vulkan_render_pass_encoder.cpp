
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

namespace vkt
{

VulkanRenderPassEncoder::VulkanRenderPassEncoder(VulkanCommandBuffer* commandBuffer, const RenderPassEncoderDescriptor& descriptor)
    : RenderPassEncoder(commandBuffer, descriptor)
{
    // TODO: multiple color attachments.
    const ColorAttachment& colorAttachment = m_descriptor.colorAttachments[0];
    auto vulkanRenderTextureView = downcast(colorAttachment.renderView);

    VulkanRenderPassDescriptor renderPassDescriptor{ .colorFormat = ToVkFormat(vulkanRenderTextureView->getFormat()),
                                                     .depthStencilFormat = m_descriptor.depthStencilAttachment.has_value() ? std::optional<VkFormat>{ ToVkFormat(descriptor.depthStencilAttachment.value().textureView->getFormat()) } : std::nullopt,
                                                     .loadOp = ToVkAttachmentLoadOp(colorAttachment.loadOp),
                                                     .storeOp = ToVkAttachmentStoreOp(colorAttachment.storeOp),
                                                     .samples = ToVkSampleCountFlagBits(vulkanRenderTextureView->getSampleCount()) };

    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    auto vulkanRenderPass = vulkanDevice->getRenderPass(renderPassDescriptor);

    bool hasDepthStencil = m_descriptor.depthStencilAttachment.has_value();
    bool hasMultiSample = vulkanRenderTextureView->getSampleCount() > 1 && colorAttachment.resolveView;

    uint64_t imageViewCount = m_descriptor.colorAttachments.size();
    imageViewCount += hasDepthStencil ? 1 : 0;
    imageViewCount += hasMultiSample ? 1 : 0;

    uint64_t imageViewIndex = 0;

    std::vector<VkImageView> imageviews{};
    imageviews.resize(imageViewCount); // set only for color and depth texture.
    imageviews[imageViewIndex++] = vulkanRenderTextureView->getVkImageView();
    if (hasDepthStencil)
    {
        auto vulkanDepthStencilTextureView = downcast(m_descriptor.depthStencilAttachment.value().textureView);
        imageviews[imageViewIndex++] = vulkanDepthStencilTextureView->getVkImageView();
    }

    if (hasMultiSample)
        imageviews[imageViewIndex++] = downcast(colorAttachment.resolveView)->getVkImageView();

    VulkanFramebufferDescriptor framebufferDescriptor{ .renderPass = vulkanRenderPass->getVkRenderPass(),
                                                       .imageViews = imageviews,
                                                       .width = vulkanRenderTextureView->getWidth(),
                                                       .height = vulkanRenderTextureView->getHeight() };
    auto vulkanFrameBuffer = vulkanDevice->getFrameBuffer(framebufferDescriptor);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->getVkRenderPass();
    renderPassInfo.framebuffer = vulkanFrameBuffer->getVkFrameBuffer();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = { vulkanFrameBuffer->getWidth(), vulkanFrameBuffer->getHeight() };

    uint64_t clearValueCount = m_descriptor.colorAttachments.size();
    clearValueCount += m_descriptor.depthStencilAttachment.has_value() ? 1 : 0;

    std::vector<VkClearValue> clearValues{};
    clearValues.resize(clearValueCount);

    uint64_t clearValueIndex = 0;

    VkClearValue colorClearValue{};
    for (uint32_t i = 0; i < 4; ++i)
    {
        colorClearValue.color.float32[i] = colorAttachment.clearValue.float32[i];
    }
    clearValues[clearValueIndex++] = colorClearValue;

    if (hasDepthStencil)
    {
        auto depthStencilClearValue = m_descriptor.depthStencilAttachment.value();
        VkClearValue depthStencilValue;
        depthStencilValue.depthStencil = { depthStencilClearValue.clearValue.depth, depthStencilClearValue.clearValue.stencil };

        clearValues[clearValueIndex++] = depthStencilValue;
    }

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vulkanDevice->vkAPI.CmdBeginRenderPass(vulkanCommandBuffer->getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanRenderPassEncoder::setPipeline(Pipeline* pipeline)
{
    m_pipeline = pipeline;

    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    VulkanRenderPipeline* vulkanRenderPipeline = downcast(static_cast<RenderPipeline*>(pipeline)); // TODO: downcasting to RenderPipeline.
    vulkanDevice->vkAPI.CmdBindPipeline(vulkanCommandBuffer->getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanRenderPipeline->getVkPipeline());
}

void VulkanRenderPassEncoder::setBindingGroup(uint32_t index, BindingGroup* bindingGroup)
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

void VulkanRenderPassEncoder::setVertexBuffer(Buffer* buffer)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanBuffer = downcast(buffer);
    VkBuffer vertexBuffers[] = { vulkanBuffer->getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vulkanDevice->vkAPI.CmdBindVertexBuffers(vulkanCommandBuffer->getVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
}

void VulkanRenderPassEncoder::setIndexBuffer(Buffer* buffer)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanBuffer = downcast(buffer);
    vulkanDevice->vkAPI.CmdBindIndexBuffer(vulkanCommandBuffer->getVkCommandBuffer(), vulkanBuffer->getVkBuffer(), 0, VK_INDEX_TYPE_UINT16);
}

void VulkanRenderPassEncoder::setViewport(float x,
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

void VulkanRenderPassEncoder::setScissor(float x,
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

void VulkanRenderPassEncoder::draw(uint32_t vertexCount)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdDraw(vulkanCommandBuffer->getVkCommandBuffer(), vertexCount, 1, 0, 0);
}

void VulkanRenderPassEncoder::drawIndexed(uint32_t indexCount)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdDrawIndexed(vulkanCommandBuffer->getVkCommandBuffer(), indexCount, 1, 0, 0, 0);
}

void VulkanRenderPassEncoder::end()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdEndRenderPass(vulkanCommandBuffer->getVkCommandBuffer());
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