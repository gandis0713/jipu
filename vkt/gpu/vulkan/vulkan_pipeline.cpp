#include "vulkan_pipeline.h"
#include "vulkan_device.h"
#include "vulkan_render_pass.h"
#include "vulkan_texture.h"

#include "utils/log.h"

#include <array>
#include <stdexcept>

namespace vkt
{

VulkanRenderPipeline::VulkanRenderPipeline(VulkanDevice* vulkanDevice, const RenderPipelineDescriptor& descriptor)
    : RenderPipeline(vulkanDevice, descriptor)
{
    initialize();
}

VulkanRenderPipeline::~VulkanRenderPipeline()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyPipeline(vulkanDevice->getVkDevice(), m_graphicsPipeline, nullptr);
    vulkanDevice->vkAPI.DestroyPipelineLayout(vulkanDevice->getVkDevice(), m_pipelineLayout, nullptr);
}

VkPipeline VulkanRenderPipeline::getVkPipeline() const
{
    return m_graphicsPipeline;
}

void VulkanRenderPipeline::initialize()
{
    // Input Assembly Stage
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = ToVkPrimitiveTopology(m_descriptor.inputAssembly.topology);
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    // Vertex Stage
    auto vertexShaderModule = downcast(m_descriptor.vertex.shader)->getVkShaderModule();

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
            attributeDescription.binding = bindingIndex;
            attributeDescription.location = attrIndex;
            attributeDescription.format = ToVkVertexFormat(attributes[attrIndex].format);
            attributeDescription.offset = attributes[attrIndex].offset;

            vertexAttributeDescriptions.push_back(attributeDescription);
        }

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = bindingIndex;
        bindingDescription.stride = m_descriptor.vertex.layouts[bindingIndex].stride;
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
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;          // Optional
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;    // Optional

    // Fragment Stage
    auto fragmentShaderModule = downcast(m_descriptor.fragment.shader)->getVkShaderModule();

    VkPipelineShaderStageCreateInfo fragmentStageInfo{};
    fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageInfo.module = fragmentShaderModule;
    fragmentStageInfo.pName = m_descriptor.fragment.entryPoint.c_str();

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
    dynamicStateCreateInfo.pDynamicStates = dynamicStates;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;            // Optional
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;         // Optional
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

    auto vulkanDevice = downcast(m_device);
    if (vulkanDevice->vkAPI.CreatePipelineLayout(vulkanDevice->getVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // TODO : multi sample
    VulkanRenderPassDescriptor renderPassDescriptor{ .format = ToVkFormat(m_descriptor.fragment.targets[0].format),
                                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                     .samples = VK_SAMPLE_COUNT_1_BIT };
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
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlendingStateCreateInfo;
    pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = vulkanRenderPass->getVkRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    if (vulkanDevice->vkAPI.CreateGraphicsPipelines(vulkanDevice->getVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
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
