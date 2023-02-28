#pragma once

#include "vk/precompile.h"
#include <string>
#include <vector>

namespace vkt
{
class Pipeline
{
public:
    Pipeline();
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    
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
