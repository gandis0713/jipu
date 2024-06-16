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

    MaliGPU(const MaliGPU&) = delete;
    MaliGPU& operator=(const MaliGPU&) = delete;

public:
    /**
     * create a sampler.
     */
    std::unique_ptr<Sampler> create(const SamplerDescriptor& descriptor) override;

    /**
     * available counters.
     */
    const std::unordered_set<Counter> counters() const override;

public:
    int deviceNumber() const;
    const std::unordered_set<hwcpipe_counter>& hwcCounters() const;

private:
    void collectCounters();

private:
    uint32_t m_id{};
    int m_deviceNumber{ -1 };
    std::unordered_set<hwcpipe_counter> m_counters{};
};

} // namespace mali
} // namespace hpc