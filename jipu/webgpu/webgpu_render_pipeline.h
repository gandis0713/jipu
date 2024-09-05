
#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/pipeline.h"

namespace jipu
{

class WebGPUDevice;
class WebGPURenderPipeline : public RefCounted
{

public:
    static WebGPURenderPipeline* create(WebGPUDevice* device, WGPURenderPipelineDescriptor const* descriptor);

public:
    WebGPURenderPipeline() = delete;
    explicit WebGPURenderPipeline(WebGPUDevice* device, std::unique_ptr<RenderPipeline> pipeline, WGPURenderPipelineDescriptor const* descriptor);

public:
    virtual ~WebGPURenderPipeline() = default;

    WebGPURenderPipeline(const WebGPURenderPipeline&) = delete;
    WebGPURenderPipeline& operator=(const WebGPURenderPipeline&) = delete;

public: // WebGPU API
private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;
    [[maybe_unused]] const WGPURenderPipelineDescriptor m_descriptor{};

private:
    std::unique_ptr<RenderPipeline> m_pipeline = nullptr;
};

} // namespace jipu