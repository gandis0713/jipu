#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/queue.h"

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

private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;

private:
    std::unique_ptr<Queue> m_queue = nullptr;
};

} // namespace jipu