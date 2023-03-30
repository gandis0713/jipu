#pragma once

#include "gpu/render_pass.h"

#include "vulkan_api.h"

namespace vkt
{
class VulkanDevice;
class VulkanRenderPass : public RenderPass
{
public:
    VulkanRenderPass(VulkanDevice* device, RenderPassDescriptor descriptor);
    ~VulkanRenderPass() override;

    VkRenderPass getRenderPass() const;

private:
    VkRenderPass m_renderPass{ nullptr };
};

} // namespace vkt
