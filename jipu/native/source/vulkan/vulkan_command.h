#pragma once

#include "vulkan_api.h"
#include <vector>

#include "jipu/render_pass_encoder.h"

namespace jipu
{

enum class VulkanCommandType
{
    kBeginRenderPass,
};

struct VulkanCommand
{
    VulkanCommandType type;
};

struct VulkanBeginRenderPassCommand : public VulkanCommand
{
    const void* next = nullptr;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkRect2D renderArea{};
    std::vector<VkClearValue> clearValues{};

    // TODO: convert timestampWrites for vulkan.
    QuerySet* occlusionQuerySet = nullptr;
    RenderPassTimestampWrites timestampWrites{};
};
} // namespace jipu