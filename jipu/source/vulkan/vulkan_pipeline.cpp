#include "vulkan_pipeline.h"
#include "vulkan_device.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include <array>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

// Vulkan Compute Pipeline
VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* device, const ComputePipelineDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
    initialize();
}

VulkanComputePipeline::~VulkanComputePipeline()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyPipeline(vulkanDevice->getVkDevice(), m_pipeline, nullptr);
}

PipelineLayout* VulkanComputePipeline::getPipelineLayout() const
{
    return m_descriptor.layout;
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
std::array<VkPipelineShaderStageCreateInfo, 2> generateShaderStageCreateInfo(const RenderPipelineDescriptor& descriptor)
{
    // Vertex Stage
    auto vertexShaderModule = downcast(descriptor.vertex.shaderModule)->getVkShaderModule();

    VkPipelineShaderStageCreateInfo vertexStageInfo{};
    vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageInfo.module = vertexShaderModule;
    vertexStageInfo.pName = descriptor.vertex.entryPoint.c_str();

    // Fragment Stage
    auto fragmentShaderModule = downcast(descriptor.fragment.shaderModule)->getVkShaderModule();

    VkPipelineShaderStageCreateInfo fragmentStageInfo{};
    fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStageInfo.module = fragmentShaderModule;
    fragmentStageInfo.pName = descriptor.fragment.entryPoint.c_str();

    return { vertexStageInfo, fragmentStageInfo };
}

std::vector<VkVertexInputBindingDescription> generateVertexInputBindingDescription(const RenderPipelineDescriptor& descriptor)
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
    for (uint64_t bindingIndex = 0; bindingIndex < descriptor.vertex.layouts.size(); ++bindingIndex)
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = static_cast<uint32_t>(bindingIndex);
        bindingDescription.stride = static_cast<uint32_t>(descriptor.vertex.layouts[bindingIndex].stride);
        bindingDescription.inputRate = ToVkVertexInputRate(descriptor.vertex.layouts[bindingIndex].mode);
        bindingDescriptions.push_back(bindingDescription);
    }

    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> generateVertexInputAttributeDescription(const RenderPipelineDescriptor& descriptor)
{
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions{};
    for (uint64_t bindingIndex = 0; bindingIndex < descriptor.vertex.layouts.size(); ++bindingIndex)
    {
        const auto& attributes = descriptor.vertex.layouts[bindingIndex].attributes;
        for (uint64_t attrIndex = 0; attrIndex < attributes.size(); ++attrIndex)
        {
            VkVertexInputAttributeDescription attributeDescription{};
            attributeDescription.binding = attributes[attrIndex].slot;
            attributeDescription.location = attributes[attrIndex].location;
            attributeDescription.format = ToVkVertexFormat(attributes[attrIndex].format);
            attributeDescription.offset = static_cast<uint32_t>(attributes[attrIndex].offset);

            vertexAttributeDescriptions.push_back(attributeDescription);
        }
    }

    return vertexAttributeDescriptions;
}

std::vector<VkPipelineColorBlendAttachmentState> generateColorBlendAttachmentState(const RenderPipelineDescriptor& descriptor)
{
    uint32_t targetSize = static_cast<uint32_t>(descriptor.fragment.targets.size());
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(targetSize);
    for (auto i = 0; i < targetSize; ++i)
    {
        auto target = descriptor.fragment.targets[i];
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = target.blend.has_value();
        if (colorBlendAttachment.blendEnable)
        {
            auto blend = target.blend.value();
            colorBlendAttachment.srcColorBlendFactor = ToVkBlendFactor(blend.color.srcFactor);
            colorBlendAttachment.dstColorBlendFactor = ToVkBlendFactor(blend.color.dstFactor);
            colorBlendAttachment.colorBlendOp = ToVkBlendOp(blend.color.operation);
            colorBlendAttachment.srcAlphaBlendFactor = ToVkBlendFactor(blend.alpha.srcFactor);
            colorBlendAttachment.dstAlphaBlendFactor = ToVkBlendFactor(blend.alpha.dstFactor);
            colorBlendAttachment.alphaBlendOp = ToVkBlendOp(blend.alpha.operation);
        }
        else
        {
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        }

        colorBlendAttachmentStates[i] = colorBlendAttachment;
    }

    return colorBlendAttachmentStates;
}

VulkanRenderPassDescriptor generateVulkanRenderPassDescriptor(const RenderPipelineDescriptor& descriptor)
{
    // Refer to render pass compatibility (https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#renderpass-compatibility)
    VulkanRenderPassDescriptor renderPassDescriptor{};

    uint32_t targetSize = static_cast<uint32_t>(descriptor.fragment.targets.size());
    renderPassDescriptor.colorAttachments.resize(targetSize);
    for (auto i = 0; i < targetSize; ++i)
    {
        const auto& target = descriptor.fragment.targets[i];
        VulkanColorAttachment& vulkanColorAttachment = renderPassDescriptor.colorAttachments[i];
        vulkanColorAttachment.format = target.format;
        vulkanColorAttachment.loadOp = LoadOp::kClear;
        vulkanColorAttachment.storeOp = StoreOp::kStore;
        vulkanColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        vulkanColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    if (descriptor.depthStencil.has_value())
    {
        const DepthStencilStage depthStencilStage = descriptor.depthStencil.value();
        VulkanDepthStencilAttachment vulkanDepthStencilAttachment{};
        vulkanDepthStencilAttachment.format = depthStencilStage.format;
        vulkanDepthStencilAttachment.depthLoadOp = LoadOp::kClear;
        vulkanDepthStencilAttachment.depthStoreOp = StoreOp::kStore;
        vulkanDepthStencilAttachment.stencilLoadOp = LoadOp::kDontCare;
        vulkanDepthStencilAttachment.stencilStoreOp = StoreOp::kDontCare;

        renderPassDescriptor.depthStencilAttachment = vulkanDepthStencilAttachment;
    }

    renderPassDescriptor.sampleCount = descriptor.rasterization.sampleCount;

    return renderPassDescriptor;
}

VulkanRenderPipeline::VulkanRenderPipeline(VulkanDevice* device, const VulkanRenderPipelineDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
    initialize();
}

VulkanRenderPipeline::~VulkanRenderPipeline()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyPipeline(vulkanDevice->getVkDevice(), m_pipeline, nullptr);
}

PipelineLayout* VulkanRenderPipeline::getPipelineLayout() const
{
    return m_descriptor.layout;
}

VkPipeline VulkanRenderPipeline::getVkPipeline() const
{
    return m_pipeline;
}

void VulkanRenderPipeline::initialize()
{
    const auto& descriptor = m_descriptor;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = ToVkPrimitiveTopology(m_descriptor.inputAssembly.topology);
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = generateShaderStageCreateInfo(descriptor);

    std::vector<VkVertexInputBindingDescription> bindingDescriptions = generateVertexInputBindingDescription(descriptor);
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = generateVertexInputAttributeDescription(descriptor);

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
    rasterizationStateCreateInfo.cullMode = ToVkCullModeFlags(descriptor.rasterization.cullMode);
    rasterizationStateCreateInfo.frontFace = ToVkFrontFace(descriptor.rasterization.frontFace);
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;          // Optional
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = ToVkSampleCountFlagBits(descriptor.rasterization.sampleCount);
    multisampleStateCreateInfo.minSampleShading = 1.0f;          // Optional
    multisampleStateCreateInfo.pSampleMask = nullptr;            // Optional
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;      // Optional

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates = generateColorBlendAttachmentState(descriptor);
    VkPipelineColorBlendStateCreateInfo colorBlendingStateCreateInfo{};
    colorBlendingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendingStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlendingStateCreateInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
    colorBlendingStateCreateInfo.pAttachments = colorBlendAttachmentStates.data();
    colorBlendingStateCreateInfo.blendConstants[0] = 0.0f; // Optional
    colorBlendingStateCreateInfo.blendConstants[1] = 0.0f; // Optional
    colorBlendingStateCreateInfo.blendConstants[2] = 0.0f; // Optional
    colorBlendingStateCreateInfo.blendConstants[3] = 0.0f; // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = descriptor.depthStencil.has_value();
    depthStencil.depthWriteEnable = descriptor.depthStencil.has_value();
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {};  // Optional

    std::array<VkDynamicState, 2> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        // VK_DYNAMIC_STATE_LINE_WIDTH,
        // VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        // VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        // VK_DYNAMIC_STATE_STENCIL_REFERENCE,
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineInfo.pViewportState = &viewportStateCreateInfo;
    pipelineInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlendingStateCreateInfo;
    pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineInfo.layout = downcast(descriptor.layout)->getVkPipelineLayout();
    pipelineInfo.renderPass = m_descriptor.renderPass;
    pipelineInfo.subpass = m_descriptor.subpassIndex;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    auto vulkanDevice = downcast(m_device);
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
    case VertexFormat::kUNORM8:
        return VK_FORMAT_R8_UNORM;
    case VertexFormat::kUNORM8x2:
        return VK_FORMAT_R8G8_UNORM;
    case VertexFormat::kUNORM8x3:
        return VK_FORMAT_R8G8B8_UNORM;
    case VertexFormat::kUNORM8x4:
        return VK_FORMAT_R8G8B8A8_UNORM;
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
        spdlog::error("{} topology for primitive is not supported.", static_cast<uint32_t>(topology));
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    }
}

VkCullModeFlags ToVkCullModeFlags(CullMode mode)
{
    VkCullModeFlags flags = 0u;
    switch (mode)
    {
    case CullMode::kFront:
        flags = VK_CULL_MODE_FRONT_BIT;
        break;
    case CullMode::kBack:
        flags = VK_CULL_MODE_BACK_BIT;
        break;
    case CullMode::kNone:
    default:
        flags = VK_CULL_MODE_NONE;
        break;
    }

    return flags;
}

VkFrontFace ToVkFrontFace(FrontFace frontFace)
{
    VkFrontFace face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    switch (frontFace)
    {
    case FrontFace::kClockwise:
        face = VK_FRONT_FACE_CLOCKWISE;
        break;
    case FrontFace::kCounterClockwise:
    default:
        face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        break;
    }

    return face;
}

VkBlendOp ToVkBlendOp(BlendOperation op)
{
    VkBlendOp blendOp = VK_BLEND_OP_ADD;

    switch (op)
    {
    default:
    case BlendOperation::kAdd:
        blendOp = VK_BLEND_OP_ADD;
        break;
    case BlendOperation::kSubtract:
        blendOp = VK_BLEND_OP_SUBTRACT;
        break;
    case BlendOperation::kMin:
        blendOp = VK_BLEND_OP_MIN;
        break;
    case BlendOperation::kMax:
        blendOp = VK_BLEND_OP_MAX;
        break;
    }

    return blendOp;
}

VkBlendFactor ToVkBlendFactor(BlendFactor factor)
{
    VkBlendFactor blendFactor = VK_BLEND_FACTOR_ONE;

    switch (factor)
    {
    default:
    case BlendFactor::kZero:
        blendFactor = VK_BLEND_FACTOR_ZERO;
        break;
    case BlendFactor::kOne:
        blendFactor = VK_BLEND_FACTOR_ONE;
        break;
    case BlendFactor::kSrcColor:
        blendFactor = VK_BLEND_FACTOR_SRC_COLOR;
        break;
    case BlendFactor::kSrcAlpha:
        blendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        break;
    case BlendFactor::kOneMinusSrcColor:
        blendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        break;
    case BlendFactor::kOneMinusSrcAlpha:
        blendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        break;
    case BlendFactor::kDstColor:
        blendFactor = VK_BLEND_FACTOR_DST_COLOR;
        break;
    case BlendFactor::kDstAlpha:
        blendFactor = VK_BLEND_FACTOR_DST_ALPHA;
        break;
    case BlendFactor::kOneMinusDstColor:
        blendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        break;
    case BlendFactor::kOneMinusDstAlpha:
        blendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        break;
    }

    return blendFactor;
}

VkVertexInputRate ToVkVertexInputRate(VertexMode mode)
{
    VkVertexInputRate inputRate;

    switch (mode)
    {
    case VertexMode::kVertex:
    default:
        inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        break;
    case VertexMode::kInstance:
        inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        break;
    }

    return inputRate;
}

} // namespace jipu
