#include "mali_sampler.h"

#include "mali_counter.h"

#include <hwcpipe/gpu.hpp>
#include <spdlog/spdlog.h>

namespace jipu
{
namespace hpc
{
namespace mali
{

MaliSampler::MaliSampler(MaliGPU gpu, SamplerDescriptor descriptor)
    : Sampler(descriptor)
    , m_gpu(gpu)
    , m_sampler(hwcpipe::sampler_config(hwcpipe::gpu(m_gpu.getDeviceNumber()))) // empty counter sampler
{
    auto config = hwcpipe::sampler_config(hwcpipe::gpu(m_gpu.getDeviceNumber()));

    std::error_code ec;
    for (const auto& counter : m_descriptor.counters)
    {
        ec = config.add_counter(counterToHwc(counter));
        if (ec)
            spdlog::error("GPU {} counter not supported by this GPU.", static_cast<uint32_t>(counter));
    }

    m_sampler = hwcpipe::sampler<>(config);
}

void MaliSampler::start()
{
    spdlog::debug("start sampling {}", __func__);
    m_sampler.start_sampling();
}

void MaliSampler::stop()
{
    spdlog::debug("stop sampling {}", __func__);
    m_sampler.stop_sampling();
}

std::vector<Sample> MaliSampler::samples(std::vector<Counter> counters)
{
    if (counters.empty())
        counters = m_descriptor.counters;

    m_sampler.sample_now();

    std::vector<hpc::Sample> samples(counters.size());
    for (auto i = 0; i < counters.size(); ++i)
    {
        const auto& counter = counters[i];
        hwcpipe::counter_sample hwcSample;
        std::error_code ec = m_sampler.get_counter_value(counterToHwc(counter), hwcSample);
        if (ec)
        {
            spdlog::error("Failed to get counter value {}", static_cast<uint32_t>(counter));
            continue;
        }

        samples[i] = { .counter = counter,
                       .timestamp = 0 };
        switch (hwcSample.type)
        {
        case hwcpipe::counter_sample::type::uint64:
            samples[i].type = Sample::Type::uint64;
            samples[i].value = Sample::Value(hwcSample.value.uint64);
            break;
        case hwcpipe::counter_sample::type::float64:
            samples[i].type = Sample::Type::float64;
            samples[i].value = Sample::Value(hwcSample.value.float64);
            break;
        }
    }

    return samples;
}

} // namespace mali
} // namespace hpc
} // namespace jipu