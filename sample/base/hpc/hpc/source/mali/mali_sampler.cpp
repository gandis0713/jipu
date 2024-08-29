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

Sample::Type convertToType([[maybe_unused]] const hpc::Counter counter, const hwcpipe::counter_sample& sample)
{
    switch (sample.type)
    {
    case hwcpipe::counter_sample::type::uint64:
        return Sample::Type::uint64;
    case hwcpipe::counter_sample::type::float64:
        return Sample::Type::float64;
    }
}

Sample::Value convertToValue([[maybe_unused]] const hpc::Counter counter, const hwcpipe::counter_sample& sample)
{
    switch (sample.type)
    {
    case hwcpipe::counter_sample::type::uint64:
        return Sample::Value(sample.value.uint64);
    case hwcpipe::counter_sample::type::float64: {
        switch (counter)
        {
        case hpc::Counter::ExternalReadBytes:
        case hpc::Counter::ExternalWriteBytes:
        case hpc::Counter::L2CacheL1Read:
        case hpc::Counter::L2CacheL1Write:
        case hpc::Counter::L2CacheRead:
        case hpc::Counter::L2CacheWrite:
        case hpc::Counter::MMUS2L3Hit:
        case hpc::Counter::MMUS2L2Hit:
        case hpc::Counter::L2ReadByte:
            return Sample::Value(sample.value.float64 / 1024.0f / 1024.0f);
        default:
            return Sample::Value(sample.value.float64);
        }
    }
    }
}

Sample convertToSample(const hpc::Counter counter, const hwcpipe::counter_sample& s)
{
    return Sample{
        .counter = counter,
        .timestamp = s.timestamp,
        .value = convertToValue(counter, s),
        .type = convertToType(counter, s)
    };
}

} // namespace

MaliSampler::MaliSampler(const MaliGPU gpu, const SamplerDescriptor& descriptor)
    : Sampler()
    , m_gpu(gpu)
    , m_sampler(hwcpipe::sampler_config(hwcpipe::gpu(m_gpu.deviceNumber()))) // empty counter sampler
    , m_descriptor(descriptor)
{
    auto config = hwcpipe::sampler_config(hwcpipe::gpu(m_gpu.deviceNumber()));

    // convert counters to hwcpipe_counter and them.
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

std::error_code MaliSampler::start()
{
    spdlog::debug("start sampling {}", __func__);

    return m_sampler.start_sampling();
}

std::error_code MaliSampler::stop()
{
    spdlog::debug("stop sampling {}", __func__);

    return m_sampler.stop_sampling();
}

Sample MaliSampler::sample(const Counter counter)
{
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
            return Sample{ .counter = counter };
        }
    }

    return convertToSample(counter, hwcSample);
}

std::vector<Sample> MaliSampler::samples(std::unordered_set<Counter> counters)
{
    if (counters.empty())
        counters = m_descriptor.counters;

    auto error = m_sampler.sample_now();
    if (error)
    {
        spdlog::error("Failed to sample counters. error {}, {}", error.value(), error.message());
        return {};
    }

    std::vector<hpc::Sample> samples{};
    for (const auto counter : counters)
    {
        samples.push_back(sample(counter));
    }

    return samples;
}

} // namespace mali
} // namespace hpc