#include "vulkan_pipeline_group.h"

#include "vulkan_device.h"

namespace jipu
{

VulkanRenderPipelineGroup::VulkanRenderPipelineGroup(VulkanDevice* device, const VulkanRenderPipelineGroupDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
    auto renderPassDescriptors = generateVulkanRenderPassDescriptor(descriptor);
    auto renderPass = m_device->getRenderPass(renderPassDescriptors);
    for (auto i = 0; i < m_descriptor.pipelines.size(); ++i)
    {
        const auto& pipelineDescriptor = m_descriptor.pipelines[i];
        VulkanRenderPipelineDescriptor vulkanRenderPipelineDescriptor{};
        vulkanRenderPipelineDescriptor.inputAssembly = pipelineDescriptor.inputAssembly;
        vulkanRenderPipelineDescriptor.vertex = pipelineDescriptor.vertex;
        vulkanRenderPipelineDescriptor.rasterization = pipelineDescriptor.rasterization;
        vulkanRenderPipelineDescriptor.depthStencil = pipelineDescriptor.depthStencil;
        vulkanRenderPipelineDescriptor.fragment = pipelineDescriptor.fragment;
        vulkanRenderPipelineDescriptor.layout = pipelineDescriptor.layout;

        vulkanRenderPipelineDescriptor.renderPass = renderPass->getVkRenderPass();
        vulkanRenderPipelineDescriptor.subpassIndex = i;

        m_pipelines.push_back(m_device->createRenderPipeline(vulkanRenderPipelineDescriptor));
    }
}

VulkanRenderPipeline* VulkanRenderPipelineGroup::getRenderPipeline(uint32_t index) const
{
    if (m_pipelines.empty())
        return nullptr;

    if (m_pipelines.size() <= index)
        return nullptr;

    return downcast(m_pipelines[index].get());
}

// Generator Helper
std::vector<VulkanRenderPassDescriptor> generateVulkanRenderPassDescriptor(const VulkanRenderPipelineGroupDescriptor& descriptor)
{
    std::vector<VulkanRenderPassDescriptor> renderPassDescriptors{};
    for (const auto& pipeline : descriptor.pipelines)
    {
        renderPassDescriptors.push_back(generateVulkanRenderPassDescriptor(pipeline));
    }

    return renderPassDescriptors;
}

} // namespace jipu