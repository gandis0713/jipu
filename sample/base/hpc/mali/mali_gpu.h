#pragma once

#include "gpu.h"

namespace jipu
{
namespace hpc
{
namespace mali
{

class MaliGPU final : public GPU
{
public:
    explicit MaliGPU(int deviceNumber);

public:
    /**
     * create a sampler.
     */
    Sampler::Ptr create(SamplerDescriptor descriptor) override;

public:
    int getDeviceNumber() const;

private:
    void collectCounters();

private:
    uint32_t id{};
    int m_deviceNumber{ -1 };

public:
    using Ptr = std::unique_ptr<MaliGPU>;
};

} // namespace mali
} // namespace hpc
} // namespace jipu