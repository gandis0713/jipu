
#pragma once

#include "common/ref_counted.h"
#include "jipu/pipeline_layout.h"
#include "webgpu_header.h"

namespace jipu
{

class WebGPUDevice;
class WebGPUPipelineLayout : public RefCounted
{

public:
    static WebGPUPipelineLayout* create(WebGPUDevice* device, WGPUPipelineLayoutDescriptor const* descriptor);

public:
    WebGPUPipelineLayout() = delete;
    explicit WebGPUPipelineLayout(WebGPUDevice* device, std::unique_ptr<PipelineLayout> layout, WGPUPipelineLayoutDescriptor const* descriptor);

public:
    virtual ~WebGPUPipelineLayout() = default;

    WebGPUPipelineLayout(const WebGPUPipelineLayout&) = delete;
    WebGPUPipelineLayout& operator=(const WebGPUPipelineLayout&) = delete;

public: // WebGPU API
private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;
    [[maybe_unused]] const WGPUPipelineLayoutDescriptor m_descriptor{};

private:
    std::unique_ptr<PipelineLayout> m_layout = nullptr;
};

} // namespace jipu