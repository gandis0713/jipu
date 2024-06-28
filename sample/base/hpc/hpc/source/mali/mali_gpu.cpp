#include "mali_gpu.h"

#include "mali_sampler.h"

#include <spdlog/spdlog.h>

#include <hwcpipe/counter_database.hpp>
#include <hwcpipe/gpu.hpp>

namespace hpc
{
namespace mali
{

MaliGPU::MaliGPU(int deviceNumber)
    : m_id{ 0 } // TODO
    , m_deviceNumber{ deviceNumber }
{
    collectCounters();
};

std::unique_ptr<Sampler> MaliGPU::create(const SamplerDescriptor& descriptor)
{
    return std::make_unique<MaliSampler>(*this, descriptor);
}

const std::unordered_set<Counter> MaliGPU::counters() const
{
    std::unordered_set<Counter> counters{};

    for (const auto& [counter, hwcCounter] : counterDependencies)
    {
        // TODO: generate counter from available hwc counters.
        counters.insert(counter);
    }

    return counters;
}

int MaliGPU::deviceNumber() const
{
    return m_deviceNumber;
}

const std::unordered_set<hwcpipe_counter>& MaliGPU::hwcCounters() const
{
    return m_counters;
}

void MaliGPU::collectCounters()
{
    auto gpu = hwcpipe::gpu(m_deviceNumber);
    if (!gpu)
    {
        // TODO: exception or not.
        spdlog::error("Mali GPU device {} is missing", m_deviceNumber);
        return;
    }

    spdlog::debug("------------------------------------------------------------");
    spdlog::debug("Mali GPU {} Supported counters:", gpu.get_device_number());
    spdlog::debug("------------------------------------------------------------");

    hwcpipe::counter_metadata meta;
    hwcpipe::counter_database counterDB{};
    for (hwcpipe_counter counter : counterDB.counters_for_gpu(gpu))
    {
        auto ec = counterDB.describe_counter(counter, meta);
        if (ec)
        {
            spdlog::warn("counter error {}", ec.message());
            continue;
        }

        spdlog::debug("    {}", meta.name);

        m_counters.insert(counter);
    }
}

} // namespace mali
} // namespace hpc