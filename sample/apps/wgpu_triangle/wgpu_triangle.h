#include "wgpu_sample.h"

#include <webgpu.h>

namespace jipu
{

class WGPUTriangleSample : public WGPUSample
{
public:
    WGPUTriangleSample() = delete;
    WGPUTriangleSample(const WGPUSampleDescriptor& descriptor);
    ~WGPUTriangleSample() override;

    void init() override;
    void update() override;
    void draw() override;

    void createShaderModule();
    void createPipelineLayout();
    void createPipeline();

private:
    WGPUPipelineLayout m_pipelineLayout = nullptr;
    WGPURenderPipeline m_renderPipeline = nullptr;
    WGPUShaderModule m_vertexShaderModule = nullptr;
    WGPUShaderModule m_fragShaderModule = nullptr;
    WGPUShaderModule m_shaderModule = nullptr;
};

} // namespace jipu