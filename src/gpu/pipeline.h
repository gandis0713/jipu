#pragma once

namespace vkt
{

class Device;
class RenderPass;

struct PipelineCreateInfo
{
};

class Pipeline
{
public:
    Pipeline() = delete;
    Pipeline(Device* device, PipelineCreateInfo info);
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void setRenderPass(RenderPass* renderPass);

protected:
    Device* m_device;
    RenderPass* m_renderPass;
};

} // namespace vkt
