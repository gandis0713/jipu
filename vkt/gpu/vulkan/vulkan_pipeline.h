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

class VKT_EXPORT VulkanRenderPipeline : public RenderPipeline
{
public:
    VulkanRenderPipeline() = delete;
    VulkanRenderPipeline(VulkanDevice* device, const RenderPipelineDescriptor& descriptor);
    ~VulkanRenderPipeline() override;

    VulkanRenderPipeline(const VulkanRenderPipeline&) = delete;
    VulkanRenderPipeline& operator=(const VulkanRenderPipeline&) = delete;

    VkPipeline getVkPipeline() const;

    void setRenderPass(VulkanRenderPass* renderPass);
    void createGraphicsPipeline();

private:
    // pipeline
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

    VulkanRenderPass* m_renderPass = nullptr;
};

DOWN_CAST(VulkanRenderPipeline, Pipeline);

} // namespace vkt
