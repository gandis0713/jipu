#pragma once

#include "gpu/pipeline.h"
#include "gpu/vulkan/vulkan_api.h"

#include <string>
#include <vector>

namespace vkt
{
class VulkanPipeline : public Pipeline
{
public:
    VulkanPipeline() = default;
    ~VulkanPipeline() override = default;

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    void destroy();

    void setDevice(VkDevice device);
    void setRenderPass(VkRenderPass renderPass);
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

    VkDevice m_device;
    VkRenderPass m_renderPass;
};
} // namespace vkt
