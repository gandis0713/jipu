#include "vulkan_command_encoder.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"

#include "utils/log.h"

#include <stdexcept>

namespace vkt
{

VulkanCommandEncoder::VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor)
    : CommandEncoder(commandBuffer, descriptor)
{
}

void VulkanCommandEncoder::startEncoding()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;                  // Optional
    commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    if (vulkanDevice->vkAPI.BeginCommandBuffer(vulkanCommandBuffer->getVkCommandBuffer(), &commandBufferBeginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin command buffer.");
    }

    VulkanRenderPassDescriptor renderPassDescriptor{};
    auto vulkanRenderPass = vulkanDevice->getRenderPass(renderPassDescriptor);

    VulkanFramebufferDescriptor framebufferDescriptor{};
    auto vulkanFrameBuffer = vulkanDevice->getFrameBuffer(framebufferDescriptor);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->getVkRenderPass();
    renderPassInfo.framebuffer = vulkanFrameBuffer->getVkFrameBuffer();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = { vulkanFrameBuffer->getWidth(), vulkanFrameBuffer->getHeight() };

    VkClearValue clearValue = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;

    vulkanDevice->vkAPI.CmdBeginRenderPass(vulkanCommandBuffer->getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandEncoder::endEncoding()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdEndRenderPass(vulkanCommandBuffer->getVkCommandBuffer());
    if (vulkanDevice->vkAPI.EndCommandBuffer(vulkanCommandBuffer->getVkCommandBuffer()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to end command buffer.");
    }
}

void VulkanCommandEncoder::setPipeline(Pipeline* pipeline)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanPipeline = downcast(pipeline);
    vulkanDevice->vkAPI.CmdBindPipeline(vulkanCommandBuffer->getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getVkPipeline());
}

void VulkanCommandEncoder::setVertexBuffer(Buffer* buffer)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanBuffer = downcast(buffer);
    VkBuffer vertexBuffers[] = { vulkanBuffer->getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vulkanDevice->vkAPI.CmdBindVertexBuffers(vulkanCommandBuffer->getVkCommandBuffer(), 0, 1, vertexBuffers, offsets);
}

void VulkanCommandEncoder::setIndexBuffer(Buffer* buffer)
{
}

void VulkanCommandEncoder::draw(uint32_t vertexCount)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdDraw(vulkanCommandBuffer->getVkCommandBuffer(), vertexCount, 1, 0, 0);
}

void VulkanCommandEncoder::drawIndexed(uint32_t indexCount)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdDrawIndexed(vulkanCommandBuffer->getVkCommandBuffer(), indexCount, 1, 0, 0, 0);
}

VkAttachmentLoadOp LoadOp2VkAttachmentLoadOp(LoadOp loadOp)
{
    switch (loadOp)
    {
    case LoadOp::kClear:
        return VK_ATTACHMENT_LOAD_OP_CLEAR;

    case LoadOp::kLoad:
        return VK_ATTACHMENT_LOAD_OP_LOAD;

    case LoadOp::kDontCare:
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

LoadOp VkAttachmentLoadOp2LoadOp(VkAttachmentLoadOp loadOp)
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
        LOG_ERROR("{} Load Op type is not supported.", loadOp);
        return LoadOp::kDontCare;
    }
}

VkAttachmentStoreOp StoreOp2VkAttachmentStoreOp(StoreOp storeOp)
{
    switch (storeOp)
    {
    case StoreOp::kStore:
        return VK_ATTACHMENT_STORE_OP_STORE;

    case StoreOp::kDontCare:
        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
}

StoreOp VkAttachmentStoreOp2StoreOp(VkAttachmentStoreOp storeOp)
{
    switch (storeOp)
    {
    case VK_ATTACHMENT_STORE_OP_STORE:
        return StoreOp::kStore;

    case VK_ATTACHMENT_STORE_OP_DONT_CARE:
        return StoreOp::kDontCare;

    default:
        LOG_ERROR("{} Store Op type is not supported.", storeOp);
        return StoreOp::kDontCare;
    }
}

} // namespace vkt