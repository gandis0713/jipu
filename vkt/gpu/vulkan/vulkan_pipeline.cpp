#include "vulkan_pipeline.h"
#include "vulkan_device.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_render_pass.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include <array>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

// Vulkan Compute Pipeline
VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* vulkanDevice, const ComputePipelineDescriptor& descriptor)
    : ComputePipeline(vulkanDevice, descriptor)
{
    initialize();
}

VulkanComputePipeline::~VulkanComputePipeline()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyPipeline(vulkanDevice->getVkDevice(), m_pipeline, nullptr);
}

VkPipeline VulkanComputePipeline::getVkPipeline() const
{
    return m_pipeline;
}

void VulkanComputePipeline::initialize()
{
    auto computeShaderModule = downcast(m_descriptor.compute.shaderModule)->getVkShaderModule();

    VkPipelineShaderStageCreateInfo computeStageInfo{};
    computeStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeStageInfo.module = computeShaderModule;
    computeStageInfo.pName = m_descriptor.compute.entryPoint.c_str();

    VkComputePipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.stage = computeStageInfo;
    pipelineCreateInfo.layout = downcast(m_descriptor.layout)->getVkPipelineLayout();
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineCreateInfo.basePipelineIndex = -1;              // Optional

    auto vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    if (VK_SUCCESS != vkAPI.CreateComputePipelines(vulkanDevice->getVkDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pipeline))
    {
        throw std::runtime_error("Failed to create compute pipelines.");
    }
}

// Vulkan Render Pipeline
VulkanRenderPipeline::VulkanRenderPipeline(VulkanDevice* vulkanDevice, const RenderPipelineDescriptor& descriptor)
    : RenderPipeline(vulkanDevice, descriptor)
{
    initialize();
}

VulkanRenderPipeline::~VulkanRenderPipeline()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyPipeline(vulkanDevice->getVkDevice(), m_pipeline, nullptr);
}

VkPipeline VulkanRenderPipeline::getVkPipeline() const
{
    return m_pipeline;
}

void VulkanRenderPipeline::initialize()
{
    // Input Assembly Stage
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = ToVkPrimitiveTopology(m_descriptor.inputAssembly.topology);
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    // Vertex Stage
    auto vertexShaderModule = downcast(m_descriptor.vertex.shaderModule)->getVkShaderModule();

    VkPipelineShaderStageCreateInfo vertexStageInfo{};
    vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageInfo.module = vertexShaderModule;
    vertexStageInfo.pName = m_descriptor.vertex.entryPoint.c_str();

    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions{};
    for (uint64_t bindingIndex = 0; bindingIndex < m_descriptor.vertex.layouts.size(); ++bindingIndex)
    {
        const auto& attributes = m_descriptor.vertex.layouts[bindingIndex].attributes;
        for (uint64_t attrIndex = 0; attrIndex < attributes.size(); ++attrIndex)
        {
            VkVertexInputAttributeDescription attributeDescription{};
            attributeDescription.binding = static_cast<uint32_t>(bindingIndex);
            attributeDescription.location = static_cast<uint32_t>(attrIndex);
            attributeDescription.format = ToVkVertexFormat(attributes[attrIndex].format);
            attributeDescription.offset = static_cast<uint32_t>(attributes[attrIndex].offset);

            vertexAttributeDescriptions.push_back(attributeDescription);
        }

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = static_cast<uint32_t>(bindingIndex);
        bindingDescription.stride = static_cast<uint32_t>(m_descriptor.vertex.layouts[bindingIndex].stride);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescriptions.push_back(bindingDescription);
    }

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
    vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = nullptr; // use dynamic state.
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = nullptr; // use dynamic state.

    // Rasterization Stage
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.lineWidth = 1.0f;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;          // Optional
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;    // Optional

    // Fragment Stage
    auto fragmentShaderModule = downcast(m_descriptor.fragment.shaderModule)->getVkShaderModule();

    VkPipelineShaderStageCreateInfo fragmentStageInfo{};
    fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageInfo.module = fragmentShaderModule;
    fragmentStageInfo.pName = m_descriptor.fragment.entryPoint.c_str();

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = ToVkSampleCountFlagBits(m_descriptor.rasterization.sampleCount);
    multisampleStateCreateInfo.minSampleShading = 1.0f;          // Optional
    multisampleStateCreateInfo.pSampleMask = nullptr;            // Optional
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

    VkPipelineColorBlendStateCreateInfo colorBlendingStateCreateInfo{};
    colorBlendingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendingStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlendingStateCreateInfo.attachmentCount = 1;
    colorBlendingStateCreateInfo.pAttachments = &colorBlendAttachment;
    colorBlendingStateCreateInfo.blendConstants[0] = 0.0f; // Optional
    colorBlendingStateCreateInfo.blendConstants[1] = 0.0f; // Optional
    colorBlendingStateCreateInfo.blendConstants[2] = 0.0f; // Optional
    colorBlendingStateCreateInfo.blendConstants[3] = 0.0f; // Optional

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        // VK_DYNAMIC_STATE_LINE_WIDTH,
        // VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        // VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        // VK_DYNAMIC_STATE_STENCIL_REFERENCE,
    };

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {};  // Optional

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
    dynamicStateCreateInfo.pDynamicStates = dynamicStates;

    auto vulkanDevice = downcast(m_device);

    // TODO: get RenderPass information from descriptor.
    VulkanRenderPassDescriptor renderPassDescriptor{ .colorFormat = ToVkFormat(m_descriptor.fragment.targets[0].format),
                                                     .depthStencilFormat = m_descriptor.depthStencil.format != TextureFormat::kUndefined ? std::optional<VkFormat>{ ToVkFormat(m_descriptor.depthStencil.format) } : std::nullopt,
                                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                     .samples = ToVkSampleCountFlagBits(m_descriptor.rasterization.sampleCount) };
    VulkanRenderPass* vulkanRenderPass = vulkanDevice->getRenderPass(renderPassDescriptor);
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertexStageInfo, fragmentStageInfo };

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineInfo.pViewportState = &viewportStateCreateInfo;
    pipelineInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlendingStateCreateInfo;
    pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineInfo.layout = downcast(m_descriptor.layout)->getVkPipelineLayout();
    pipelineInfo.renderPass = vulkanRenderPass->getVkRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    if (vulkanDevice->vkAPI.CreateGraphicsPipelines(vulkanDevice->getVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

VkFormat ToVkVertexFormat(VertexFormat format)
{
    switch (format)
    {
    case VertexFormat::kSFLOAT:
        return VK_FORMAT_R32_SFLOAT;

    case VertexFormat::kSFLOATx2:
        return VK_FORMAT_R32G32_SFLOAT;

    case VertexFormat::kSFLOATx3:
        return VK_FORMAT_R32G32B32_SFLOAT;

    case VertexFormat::kSFLOATx4:
        return VK_FORMAT_R32G32B32A32_SFLOAT;

    case VertexFormat::kSINT:
        return VK_FORMAT_R32_SINT;

    case VertexFormat::kSINTx2:
        return VK_FORMAT_R32G32_SINT;

    case VertexFormat::kSINTx3:
        return VK_FORMAT_R32G32B32_SINT;

    case VertexFormat::kSINTx4:
        return VK_FORMAT_R32G32B32A32_SINT;

    case VertexFormat::kUINT:
        return VK_FORMAT_R32_UINT;

    case VertexFormat::kUINTx2:
        return VK_FORMAT_R32G32_UINT;

    case VertexFormat::kUINTx3:
        return VK_FORMAT_R32G32B32_UINT;

    case VertexFormat::kUINTx4:
        return VK_FORMAT_R32G32B32A32_UINT;

    default:
        spdlog::error("{} vertex format is not supported.", static_cast<uint32_t>(format));
        return VK_FORMAT_UNDEFINED;
    }
}

VkPrimitiveTopology ToVkPrimitiveTopology(PrimitiveTopology topology)
{
    switch (topology)
    {
    case PrimitiveTopology::kPointList:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

    case PrimitiveTopology::kLineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

    case PrimitiveTopology::kLineList:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

    case PrimitiveTopology::kTriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    case PrimitiveTopology::kTriangleList:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    default:
        spdlog::error("{} topology is not supported.", static_cast<uint32_t>(topology));
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    }
}

} // namespace vkt
