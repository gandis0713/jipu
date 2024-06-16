#include "mali_sampler.h"

#include "mali_counter.h"

#include <hwcpipe/gpu.hpp>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace hpc
{
namespace mali
{

namespace
{

Sample::Type convertToType(const hwcpipe::counter_sample& sample)
{
    switch (sample.type)
    {
    case hwcpipe::counter_sample::type::uint64:
        return Sample::Type::uint64;
    case hwcpipe::counter_sample::type::float64:
        return Sample::Type::float64;
    }
}

Sample::Value convertToValue(const hwcpipe::counter_sample& sample)
{
    switch (sample.type)
    {
    case hwcpipe::counter_sample::type::uint64:
        return Sample::Value(sample.value.uint64);
    case hwcpipe::counter_sample::type::float64:
        return Sample::Value(sample.value.float64);
    }
}

} // namespace

MaliSampler::MaliSampler(const MaliGPU& gpu, const SamplerDescriptor& descriptor)
    : Sampler(descriptor)
    , m_gpu(gpu)
    , m_sampler(hwcpipe::sampler_config(hwcpipe::gpu(m_gpu.deviceNumber()))) // empty counter sampler
{
    auto config = hwcpipe::sampler_config(hwcpipe::gpu(m_gpu.deviceNumber()));

    // convert counters to hwcpipe_counter and and them.
    std::error_code ec;
    for (Counter counter : m_descriptor.counters)
    {
        auto it = counterDependencies.find(counter);
        if (it != counterDependencies.end())
        {
            const auto& dependencies = counterDependencies.at(counter);
            for (const auto& dependency : dependencies)
            {
                ec = config.add_counter(dependency);
                if (ec)
                    spdlog::error("GPU {} counter not supported by this GPU.", static_cast<std::underlying_type_t<Counter>>(counter));
            }
        }
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

Sample MaliSampler::sample(const Counter counter)
{
    Sample sample = { .counter = counter };

    hwcpipe::counter_sample hwcSample;

    auto it = counterDependencies.find(counter);
    if (it != counterDependencies.end())
    {
        const auto& dependencies = counterDependencies.at(counter);
        if (dependencies.size() != 1)
        {
            // TODO
            throw std::runtime_error("currently, not supported.");
        }

        const auto& hwcCounter = dependencies[0];
        std::error_code ec = m_sampler.get_counter_value(hwcCounter, hwcSample);
        if (ec)
        {
            spdlog::error("Failed to get counter value {}", static_cast<uint32_t>(hwcCounter));
            return sample;
        }
    }

    sample.timestamp = hwcSample.timestamp;
    sample.type = convertToType(hwcSample);
    sample.value = convertToValue(hwcSample);

    return sample;
}

std::vector<Sample> MaliSampler::samples(std::vector<Counter> counters)
{
    if (counters.empty())
        counters = m_descriptor.counters;

    m_sampler.sample_now();

    std::vector<hpc::Sample> samples(counters.size());
    for (auto i = 0; i < counters.size(); ++i)
    {
        samples[i] = sample(counters[i]);
    }

    return samples;
}

uint64_t MaliSampler::time()
{
    return 0;
}

} // namespace mali
} // namespace hpc