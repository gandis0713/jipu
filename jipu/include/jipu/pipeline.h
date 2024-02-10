#pragma once

#include "export.h"
#include "pipeline_layout.h"
#include "shader_module.h"
#include "texture.h"

#include <optional>
#include <string>
#include <vector>

namespace jipu
{

class Device;
class JIPU_EXPORT Pipeline
{
public:
    Pipeline() = delete;
    Pipeline(Device* device);
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    virtual PipelineLayout* getPipelineLayout(uint32_t index = 0) const = 0;

protected:
    Device* m_device = nullptr;
};

struct ProgrammableStage
{
    ShaderModule* shaderModule = nullptr;
    std::string entryPoint = "main";
};

/**
 * @brief Render Pipeline
 */

// Input Assembly
enum class PrimitiveTopology
{
    kUndefined = 0,
    kPointList,
    kLineStrip,
    kLineList,
    kTriangleStrip,
    kTriangleList
};

struct InputAssemblyStage
{
    PrimitiveTopology topology = PrimitiveTopology::kUndefined;
};

// Vertex Stage
enum class VertexFormat
{
    kUndefined = 0,
    kUINT,
    kUINTx2,
    kUINTx3,
    kUINTx4,
    kSINT,
    kSINTx2,
    kSINTx3,
    kSINTx4,
    kSFLOAT,
    kSFLOATx2,
    kSFLOATx3,
    kSFLOATx4,
    kUNORM8,
    kUNORM8x2,
    kUNORM8x3,
    kUNORM8x4
};

enum class VertexMode
{
    kVertex = 0,
    kInstance,
};

struct VertexAttribute
{
    VertexFormat format = VertexFormat::kUndefined;
    uint64_t offset = 0u;
    uint32_t location = 0u;
    uint32_t slot = 0u;
};

struct VertexInputLayout
{
    VertexMode mode = VertexMode::kVertex;
    uint64_t stride = 0u;
    std::vector<VertexAttribute> attributes{};
};

struct VertexStage : ProgrammableStage
{
    std::vector<VertexInputLayout> layouts{};
};

// Rasterization Stage
enum class CullMode
{
    kNone,
    kFront,
    kBack
};

enum class FrontFace
{
    kCounterClockwise,
    kClockwise
};

struct RasterizationStage
{
    uint32_t sampleCount = 0;
    CullMode cullMode = CullMode::kNone;
    FrontFace frontFace = FrontFace::kCounterClockwise;
};

enum class BlendOperation
{
    kAdd = 0,
    kSubtract,
    kMin,
    kMax,
};

enum class BlendFactor
{
    kZero = 0,
    kOne,
    kSrcColor,
    kSrcAlpha,
    kOneMinusSrcColor,
    kOneMinusSrcAlpha,
    kDstColor,
    kDstAlpha,
    kOneMinusDstColor,
    kOneMinusDstAlpha,
};

struct BlentComponent
{
    BlendFactor srcFactor = BlendFactor::kOne;
    BlendFactor dstFactor = BlendFactor::kZero;
    BlendOperation operation = BlendOperation::kAdd;
};

struct BlendState
{
    BlentComponent color;
    BlentComponent alpha;
};

// Fragment Shader Stage
struct FragmentStage : ProgrammableStage
{
    struct Target
    {
        TextureFormat format = TextureFormat::kUndefined;
        // TextureUsageFlags usage = TextureUsageFlagBits::kUndefined;
        std::optional<BlendState> blend = std::nullopt;
    };

    std::vector<FragmentStage::Target> targets{};
};

// Depth/Stencil Stage
struct DepthStencilStage
{
    /// @brief depth/stencil attachment format.
    TextureFormat format;
};

struct PipelineDescriptor
{
    /// @brief pipeline layout
    PipelineLayout* layout = nullptr;
};

struct RenderPipelineDescriptor : PipelineDescriptor
{
    InputAssemblyStage inputAssembly{};
    VertexStage vertex{};
    RasterizationStage rasterization{};
    FragmentStage fragment{};
    std::optional<DepthStencilStage> depthStencil = std::nullopt;
};

class JIPU_EXPORT RenderPipeline : public Pipeline
{
public:
    RenderPipeline() = delete;
    RenderPipeline(Device* device, const RenderPipelineDescriptor& descriptor);
    RenderPipeline(Device* device, const std::vector<RenderPipelineDescriptor>& descriptors);
    virtual ~RenderPipeline() = default;

    RenderPipeline(const RenderPipeline&) = delete;
    RenderPipeline& operator=(const RenderPipeline&) = delete;

    PipelineLayout* getPipelineLayout(uint32_t index = 0) const override;

protected:
    const std::vector<RenderPipelineDescriptor> m_descriptors{};
};

// Compute Shader Stage
struct ComputeStage : ProgrammableStage
{
};

struct ComputePipelineDescriptor : PipelineDescriptor
{
    ComputeStage compute{};
};

class JIPU_EXPORT ComputePipeline : public Pipeline
{
public:
    ComputePipeline() = delete;
    ComputePipeline(Device* device, const ComputePipelineDescriptor& descriptor);
    virtual ~ComputePipeline() = default;

    ComputePipeline(const ComputePipeline&) = delete;
    ComputePipeline& operator=(const ComputePipeline&) = delete;

    PipelineLayout* getPipelineLayout(uint32_t index = 0) const override;

protected:
    const ComputePipelineDescriptor m_descriptor{};
};

} // namespace jipu
