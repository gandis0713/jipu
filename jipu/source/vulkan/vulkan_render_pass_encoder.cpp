
#include "vulkan_render_pass_encoder.h"

#include "vulkan_binding_group.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_render_pass.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include <optional>
#include <spdlog/spdlog.h>

namespace jipu
{

namespace
{

VkImageLayout getInitialLayout(const ColorAttachment& colorAttachment)
{
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (colorAttachment.loadOp == LoadOp::kLoad)
    {
        auto renderView = colorAttachment.resolveView ? colorAttachment.resolveView : colorAttachment.renderView;

        layout = downcast(renderView->getTexture())->getFinalLayout();
    }

    return layout;
}

} // namespace

VulkanRenderPassEncoder::VulkanRenderPassEncoder(VulkanCommandBuffer* commandBuffer, const RenderPassDescriptor& descriptor)
    : RenderPassEncoder(commandBuffer, descriptor)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanRenderPass = getVulkanRenderPass(vulkanDevice, descriptor);
    auto vulkanFrameBuffer = getVulkanFramebuffer(vulkanDevice, vulkanRenderPass, descriptor);

    // get clear color
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

    std::vector<VkClearValue> clearValues{};
    addColorClearValue(clearValues, m_descriptor.colorAttachments);
    if (descriptor.sampleCount > 1)
    {
        addColorClearValue(clearValues, m_descriptor.colorAttachments);
    }

    if (m_descriptor.depthStencilAttachment.has_value())
    {
        auto depthStencilAttachment = m_descriptor.depthStencilAttachment.value();
        if (depthStencilAttachment.depthLoadOp == LoadOp::kClear || depthStencilAttachment.stencilLoadOp == LoadOp::kClear)
        {
            VkClearValue depthStencilClearValue{};
            depthStencilClearValue.depthStencil = { depthStencilAttachment.clearValue.depth,
                                                    depthStencilAttachment.clearValue.stencil };

            clearValues.push_back(depthStencilClearValue);
        }
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanRenderPass->getVkRenderPass();
    renderPassInfo.framebuffer = vulkanFrameBuffer->getVkFrameBuffer();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = { vulkanFrameBuffer->getWidth(), vulkanFrameBuffer->getHeight() };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vulkanDevice->vkAPI.CmdBeginRenderPass(vulkanCommandBuffer->getVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

VulkanRenderPassEncoder::VulkanRenderPassEncoder(VulkanCommandBuffer* commandBuffer, const std::vector<RenderPassDescriptor>& descriptors)
    : RenderPassEncoder(commandBuffer, descriptors)
{
}

void VulkanRenderPassEncoder::setPipeline(Pipeline* pipeline)
{
    // TODO: receive RenderPipeline from parameter.
    m_pipeline = pipeline;

    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    VulkanRenderPipeline* vulkanRenderPipeline = downcast(static_cast<RenderPipeline*>(pipeline)); // TODO: not casting to RenderPipeline.
    vulkanDevice->vkAPI.CmdBindPipeline(vulkanCommandBuffer->getVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanRenderPipeline->getVkPipeline());
}

void VulkanRenderPassEncoder::setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset)
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
                                static_cast<uint32_t>(dynamicOffset.size()),
                                dynamicOffset.data());
}

void VulkanRenderPassEncoder::setVertexBuffer(uint32_t slot, Buffer* buffer)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanBuffer = downcast(buffer);
    VkBuffer vertexBuffers[] = { vulkanBuffer->getVkBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vulkanDevice->vkAPI.CmdBindVertexBuffers(vulkanCommandBuffer->getVkCommandBuffer(), slot, 1, vertexBuffers, offsets);
}

void VulkanRenderPassEncoder::setIndexBuffer(Buffer* buffer, IndexFormat format)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    auto vulkanBuffer = downcast(buffer);
    vulkanDevice->vkAPI.CmdBindIndexBuffer(vulkanCommandBuffer->getVkCommandBuffer(), vulkanBuffer->getVkBuffer(), 0, ToVkIndexType(format));
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

void VulkanRenderPassEncoder::drawIndexed(uint32_t indexCount,
                                          uint32_t instanceCount,
                                          uint32_t indexOffset,
                                          uint32_t vertexOffset,
                                          uint32_t firstInstance)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdDrawIndexed(vulkanCommandBuffer->getVkCommandBuffer(),
                                       indexCount,
                                       instanceCount,
                                       indexOffset,
                                       vertexOffset,
                                       firstInstance);
}

void VulkanRenderPassEncoder::end()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());

    vulkanDevice->vkAPI.CmdEndRenderPass(vulkanCommandBuffer->getVkCommandBuffer());

    // TODO: generate stage from binding group.
    VkPipelineStageFlags flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    vulkanCommandBuffer->setSignalPipelineStage(flags);
}

// Generate Helper
VulkanRenderPass* getVulkanRenderPass(VulkanDevice* vulkanDevice, const RenderPassDescriptor& descriptor)
{
    VulkanRenderPassDescriptor renderPassDescriptor{};

    uint32_t colorAttachmentSize = static_cast<uint32_t>(descriptor.colorAttachments.size());
    renderPassDescriptor.colorAttachments.resize(colorAttachmentSize);
    for (auto i = 0; i < colorAttachmentSize; ++i)
    {
        const auto& colorAttachment = descriptor.colorAttachments[i];
        const auto texture = colorAttachment.renderView->getTexture();
        VulkanColorAttachment& vulkanColorAttachment = renderPassDescriptor.colorAttachments[i];
        vulkanColorAttachment.format = texture->getFormat();
        vulkanColorAttachment.loadOp = colorAttachment.loadOp;
        vulkanColorAttachment.storeOp = colorAttachment.storeOp;
        vulkanColorAttachment.initialLayout = getInitialLayout(colorAttachment);
        vulkanColorAttachment.finalLayout = downcast(texture)->getFinalLayout();
    }

    if (descriptor.depthStencilAttachment.has_value())
    {
        const DepthStencilAttachment depthStencilAttachment = descriptor.depthStencilAttachment.value();
        VulkanDepthStencilAttachment vulkanDepthStencilAttachment{};
        vulkanDepthStencilAttachment.format = depthStencilAttachment.textureView->getTexture()->getFormat();
        vulkanDepthStencilAttachment.depthLoadOp = depthStencilAttachment.depthLoadOp;
        vulkanDepthStencilAttachment.depthStoreOp = depthStencilAttachment.depthStoreOp;
        vulkanDepthStencilAttachment.stencilLoadOp = depthStencilAttachment.stencilLoadOp;
        vulkanDepthStencilAttachment.stencilStoreOp = depthStencilAttachment.stencilStoreOp;

        renderPassDescriptor.depthStencilAttachment = vulkanDepthStencilAttachment;
    }

    if (descriptor.colorAttachments.empty())
        throw std::runtime_error("Failed to create vulkan render pass encoder due to empty color attachment.");

    renderPassDescriptor.sampleCount = descriptor.sampleCount;

    return vulkanDevice->getRenderPass({ renderPassDescriptor });
}

VulkanFramebuffer* getVulkanFramebuffer(VulkanDevice* vulkanDevice, VulkanRenderPass* vulkanRenderPass, const RenderPassDescriptor& descriptor)
{
    VulkanFramebufferDescriptor framebufferDescriptor{};
    framebufferDescriptor.renderPass = vulkanRenderPass;

    uint32_t colorAttachmentSize = static_cast<uint32_t>(descriptor.colorAttachments.size());

    auto& textureViews = framebufferDescriptor.textureViews;
    for (auto i = 0; i < colorAttachmentSize; ++i)
    {
        const auto& colorAttachment = descriptor.colorAttachments[i];
        textureViews.push_back(colorAttachment.renderView);
    }

    if (descriptor.sampleCount > 1)
    {
        for (auto i = 0; i < colorAttachmentSize; ++i)
        {
            const auto& colorAttachment = descriptor.colorAttachments[i];
            textureViews.push_back(colorAttachment.resolveView);
        }
    }

    if (descriptor.depthStencilAttachment.has_value())
    {
        const DepthStencilAttachment depthStencilAttachment = descriptor.depthStencilAttachment.value();
        textureViews.push_back(depthStencilAttachment.textureView);
    }

    return vulkanDevice->getFrameBuffer(framebufferDescriptor);
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