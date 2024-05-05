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
    : id{ 0 } // TODO
    , m_deviceNumber{ deviceNumber }
{
    collectCounters();
};

Sampler::Ptr MaliGPU::create(SamplerDescriptor descriptor)
{
    return std::make_unique<MaliSampler>(*this, descriptor);
}

const std::vector<Counter> MaliGPU::counters() const
{
    // TODO: generate by hwcpipe_counters.
    return {
        Counter::NonFragmentUtilization,
        Counter::FragmentUtilization,
        Counter::TilerUtilization,
        Counter::ExternalReadBytes,
        Counter::ExternalWriteBytes,
        Counter::ExternalReadStallRate,
        Counter::ExternalWriteStallRate,
        Counter::ExternalReadLatency0,
        Counter::ExternalReadLatency1,
        Counter::ExternalReadLatency2,
        Counter::ExternalReadLatency3,
        Counter::ExternalReadLatency4,
        Counter::ExternalReadLatency5,
        Counter::GeometryTotalInputPrimitives
    };
}

int MaliGPU::getDeviceNumber() const
{
    return m_deviceNumber;
}

const std::vector<hwcpipe_counter>& MaliGPU::hwcCounters() const
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

        m_counters.push_back(counter);
    }
}

} // namespace mali
} // namespace hpc