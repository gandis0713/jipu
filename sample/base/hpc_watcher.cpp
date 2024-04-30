#include "hpc_watcher.h"

#include <spdlog/spdlog.h>

using namespace std::chrono;

namespace jipu
{

namespace
{

float convertToFloat(const hpc::Sample& sample)
{
    switch (sample.type)
    {
    case hpc::Sample::Type::uint64:
        return static_cast<float>(sample.value.uint64);
    case hpc::Sample::Type::float64:
        return static_cast<float>(sample.value.float64);
    }
}

float fragmentQueueUtilization(std::unordered_map<hpc::Counter, hpc::Sample> samples)
{
    auto it = samples.find(hpc::Counter::FragQueueActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for FragQueueActiveCy");
        return 0.0f;
    }

    it = samples.find(hpc::Counter::GPUActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for GPUActiveCy");
        return 0.0f;
    }

    auto value = (convertToFloat(samples[hpc::Counter::FragQueueActiveCy]) / convertToFloat(samples[hpc::Counter::GPUActiveCy])) * 100;
    return std::max(std::min(value, 100.0f), 0.0f);
}

float nonFragmentQueueUtilization(std::unordered_map<hpc::Counter, hpc::Sample> samples)
{
    auto it = samples.find(hpc::Counter::NonFragQueueActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for NonFragQueueActiveCy");
        return 0.0f;
    }

    it = samples.find(hpc::Counter::GPUActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for GPUActiveCy");
        return 0.0f;
    }

    float value = (convertToFloat(samples[hpc::Counter::NonFragQueueActiveCy]) / convertToFloat(samples[hpc::Counter::GPUActiveCy])) * 100;
    return std::max(std::min(value, 100.0f), 0.0f);
}

float tilerUtilization(std::unordered_map<hpc::Counter, hpc::Sample> samples)
{
    auto it = samples.find(hpc::Counter::TilerActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for TilerActiveCy");
        return 0.0f;
    }

    it = samples.find(hpc::Counter::GPUActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for GPUActiveCy");
        return 0.0f;
    }

    auto value = (convertToFloat(samples[hpc::Counter::TilerActiveCy]) / convertToFloat(samples[hpc::Counter::GPUActiveCy])) * 100;
    return std::max(std::min(value, 100.0f), 0.0f);
}

} // namespace

HPCWatcher::HPCWatcher(HPCWatcherDescriptor descriptor)
    : m_descriptor(std::move(descriptor))
    , m_time(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()))
{
}

void HPCWatcher::start()
{
    m_descriptor.sampler->start();
}

void HPCWatcher::stop()
{
    m_descriptor.sampler->stop();
}

void HPCWatcher::update()
{
    auto currentTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_time).count();

    if (durationTime < period)
        return;

    auto samples = m_descriptor.sampler->samples();
    {
        std::unordered_map<hpc::Counter, hpc::Sample> sampleMap{};
        for (const auto& sample : samples)
        {
            sampleMap.insert({ sample.counter, sample });

            if (false)
            {
                switch (sample.type)
                {
                case hpc::Sample::Type::uint64:
                    spdlog::error("{}: {}", hpc::counterName(sample.counter), sample.value.uint64);
                    break;
                case hpc::Sample::Type::float64:
                    spdlog::error("{}: {:.3f}", hpc::counterName(sample.counter), sample.value.float64);
                    break;
                }
            }
        }

        std::unordered_map<Counter, float> values{};
        for (const auto& counter : m_descriptor.counters)
        {
            switch (counter)
            {
            case Counter::FragmentUtilization:
                values[counter] = fragmentQueueUtilization(sampleMap);
                break;
            case Counter::NonFragmentUtilization:
                values[counter] = nonFragmentQueueUtilization(sampleMap);
                break;
            case Counter::TilerUtilization:
                values[counter] = tilerUtilization(sampleMap);
                break;
            }
        }
        m_descriptor.listner(values);

        m_time = currentTime;
    }
}

} // namespace jipu