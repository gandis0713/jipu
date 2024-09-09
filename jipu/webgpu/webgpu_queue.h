#pragma once

#include "common/ref_counted.h"
#include "jipu/queue.h"
#include "webgpu_header.h"

namespace jipu
{

class WebGPUDevice;
class WebGPUQueue : public RefCounted
{
public:
    static WebGPUQueue* create(WebGPUDevice* wgpuDevice);

public:
    WebGPUQueue() = delete;
    explicit WebGPUQueue(WebGPUDevice* wgpuDevice, std::unique_ptr<Queue> queue);

public:
    virtual ~WebGPUQueue() = default;

    WebGPUQueue(const WebGPUQueue&) = delete;
    WebGPUQueue& operator=(const WebGPUQueue&) = delete;

public: // WebGPU API
    void submit(size_t commandCount, WGPUCommandBuffer const* commands);

private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;

private:
    std::unique_ptr<Queue> m_queue = nullptr;
};

} // namespace jipu