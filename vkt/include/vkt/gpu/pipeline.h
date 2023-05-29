#pragma once

#include "export.h"
#include "shader_module.h"
#include "texture.h"

#include <string>
#include <vector>

namespace vkt
{

class Device;
class VKT_EXPORT Pipeline
{
public:
    Pipeline() = delete;
    Pipeline(Device* device);
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

protected:
    Device* m_device = nullptr;
};

struct ProgrammableStage
{
    ShaderModule* shader = nullptr;
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
};

struct VertexBindingLayout
{
    VertexMode mode = VertexMode::kVertex;
    uint64_t stride = 0u;
    std::vector<VertexAttribute> attributes{};
};

struct VertexStage : ProgrammableStage
{
    std::vector<VertexBindingLayout> layouts{};
};

// Rasterization Stage
struct RasterizationStage
{
};

// Fragment Shader Stage
struct FragmentStage : ProgrammableStage
{
    struct Target
    {
        TextureFormat format;
    };

    std::vector<FragmentStage::Target> targets{};
};

struct RenderPipelineDescriptor
{
    InputAssemblyStage inputAssembly{};
    VertexStage vertex{};
    RasterizationStage rasterization{};
    FragmentStage fragment{};
};

class VKT_EXPORT RenderPipeline : public Pipeline
{
public:
    RenderPipeline() = delete;
    RenderPipeline(Device* device, const RenderPipelineDescriptor& descriptor);
    virtual ~RenderPipeline() = default;

    RenderPipeline(const RenderPipeline&) = delete;
    RenderPipeline& operator=(const RenderPipeline&) = delete;

protected:
    const RenderPipelineDescriptor m_descriptor{};
};

} // namespace vkt
