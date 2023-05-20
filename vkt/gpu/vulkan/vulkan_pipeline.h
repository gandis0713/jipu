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

class VKT_EXPORT VulkanPipeline : public Pipeline
{
public:
    VulkanPipeline() = delete;
    VulkanPipeline(VulkanDevice* device, const PipelineDescriptor& descriptor);
    ~VulkanPipeline() override;

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    VkPipeline getVkPipeline() const;

    void setRenderPass(VulkanRenderPass* renderPass);
    void createGraphicsPipeline();

private:
    // pipeline
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

    VulkanRenderPass* m_renderPass = nullptr;
};

DOWN_CAST(VulkanPipeline, Pipeline);

} // namespace vkt
