#pragma once

#include "hpc/gpu.h"

#include "mali_counter.h"

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
    std::unique_ptr<Sampler> create(const SamplerDescriptor& descriptor) override;

    /**
     * available counters.
     */
    const std::vector<Counter> counters() const override;

public:
    int getDeviceNumber() const;
    const std::vector<hwcpipe_counter>& hwcCounters() const;

private:
    void collectCounters();

private:
    uint32_t id{};
    int m_deviceNumber{ -1 };
    std::vector<hwcpipe_counter> m_counters{};
};

} // namespace mali
} // namespace hpc