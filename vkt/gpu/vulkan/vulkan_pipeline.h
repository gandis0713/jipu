#pragma once

#include "utils/cast.h"
#include "vkt/gpu/pipeline.h"
#include "vulkan_api.h"
#include "vulkan_shader_module.h"

#include <string>
#include <vector>

namespace vkt
{

class VulkanDevice;
class VulkanRenderPass;

class VulkanRenderPipeline : public RenderPipeline
{
public:
    VulkanRenderPipeline() = delete;
    VulkanRenderPipeline(VulkanDevice* device, const RenderPipelineDescriptor& descriptor);
    ~VulkanRenderPipeline() override;

    VulkanRenderPipeline(const VulkanRenderPipeline&) = delete;
    VulkanRenderPipeline& operator=(const VulkanRenderPipeline&) = delete;

    VkPipeline getVkPipeline() const;

private:
    void initialize();

private:
    // pipeline
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
};

DOWN_CAST(VulkanRenderPipeline, Pipeline);

} // namespace vkt
