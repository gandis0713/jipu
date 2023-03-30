#pragma once

#include "gpu/pipeline.h"
#include "vulkan_api.h"

#include <string>
#include <vector>

namespace vkt
{

class VulkanDevice;

class VulkanPipeline : public Pipeline
{
public:
    VulkanPipeline() = delete;
    VulkanPipeline(VulkanDevice* device, PipelineDescriptor descriptor);
    ~VulkanPipeline() override = default;

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    void destroy();

    void bindPipeline(VkCommandBuffer commandBuffer);

    void createGraphicsPipeline(const std::string& vertShaderPath, const std::string& fragShaderPath);
    VkShaderModule createShaderModule(const std::vector<char>& codeBuffer);

private:
    // shader module
    VkShaderModule m_vertShaderModule;
    VkShaderModule m_fragShaderModule;

    // pipeline
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
};
} // namespace vkt
