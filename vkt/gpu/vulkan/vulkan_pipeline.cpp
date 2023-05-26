#include "vulkan_pipeline.h"
#include "vulkan_device.h"
#include "vulkan_render_pass.h"
#include "vulkan_texture.h"

#include <array>
#include <stddef.h>
#include <stdexcept>

namespace vkt
{

VulkanRenderPipeline::VulkanRenderPipeline(VulkanDevice* vulkanDevice, const RenderPipelineDescriptor& info)
    : RenderPipeline(vulkanDevice, info)
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
    auto vertexShaderModule = downcast(m_descriptor.vertex.shader)->getVkShaderModule();
    auto fragmentShaderModule = downcast(m_descriptor.fragment.shader)->getVkShaderModule();

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = vertexShaderModule;
    vertexShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
    fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = fragmentShaderModule;
    fragmentShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

    // TODO: remove struct
    struct Vertex
    {
        float pos[2];
        float color[3];
    };
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = 800;  // TODO: dynamic state
    viewport.height = 600; // TODO: dynamic state
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = { 800, 600 };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.lineWidth = 5.0f;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;          // Optional
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;    // Optional

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
        VK_DYNAMIC_STATE_LINE_WIDTH,
        VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        VK_DYNAMIC_STATE_STENCIL_REFERENCE,
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
                                                     .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                     .samples = VK_SAMPLE_COUNT_1_BIT };

    auto vulkanRenderPass = vulkanDevice->getRenderPass(renderPassDescriptor);
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
    pipelineInfo.pDynamicState = nullptr; // &dynamicStateCreateInfo
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

} // namespace vkt
