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

class PipelineLayout;
class JIPU_EXPORT Pipeline
{
public:
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

protected:
    Pipeline() = default;

public:
    virtual PipelineLayout& getPipelineLayout() const = 0;
};

struct ProgrammableStage
{
    ShaderModule& shaderModule;
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
    kReversSubtract,
    kMin,
    kMax,
};

enum class BlendFactor
{
    kZero = 0,
    kOne,
    kSrcColor,
    kSrcAlpha,
    kSrcAlphaSarurated,
    kOneMinusSrcColor,
    kOneMinusSrcAlpha,
    kDstColor,
    kDstAlpha,
    kOneMinusDstColor,
    kOneMinusDstAlpha,
    kConstantColor,
    kOneMinusConstantColor,

    // dual source blending
    kSrc1Color,
    kOneMinusSrc1Color,
    kSrc1Alpha,
    kOneMinusSrc1Alpha,
};

struct BlendComponent
{
    BlendFactor srcFactor = BlendFactor::kOne;
    BlendFactor dstFactor = BlendFactor::kZero;
    BlendOperation operation = BlendOperation::kAdd;
};

struct BlendState
{
    BlendComponent color;
    BlendComponent alpha;
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
    PipelineLayout& layout;
};

struct RenderPipelineDescriptor : PipelineDescriptor
{
    InputAssemblyStage inputAssembly{};
    VertexStage vertex;
    RasterizationStage rasterization{};
    FragmentStage fragment;
    std::optional<DepthStencilStage> depthStencil = std::nullopt;
};

class Device;
class JIPU_EXPORT RenderPipeline : public Pipeline
{
public:
    virtual ~RenderPipeline() = default;

    RenderPipeline(const RenderPipeline&) = delete;
    RenderPipeline& operator=(const RenderPipeline&) = delete;

protected:
    RenderPipeline() = default;
};

// Compute Shader Stage
struct ComputeStage : ProgrammableStage
{
};

struct ComputePipelineDescriptor : PipelineDescriptor
{
    ComputeStage compute;
};

class JIPU_EXPORT ComputePipeline : public Pipeline
{
public:
    virtual ~ComputePipeline() = default;

    ComputePipeline(const ComputePipeline&) = delete;
    ComputePipeline& operator=(const ComputePipeline&) = delete;

protected:
    ComputePipeline() = default;
};

} // namespace jipu
