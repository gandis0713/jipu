#include "hpc_watcher.h"

#include <spdlog/spdlog.h>

using namespace std::chrono;

namespace jipu
{

namespace
{

double convertToDouble(const hpc::Sample& sample)
{
    switch (sample.type)
    {
        case hpc::Sample::Type::uint64:
            return static_cast<double>(sample.value.uint64);
        case hpc::Sample::Type::float64:
            return sample.value.float64;
    }
}

double fragmentQueueUtilization(std::unordered_map<hpc::Counter, hpc::Sample> samples)
{
    auto it = samples.find(hpc::Counter::FragQueueActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for FragQueueActiveCy");
        return 0.0;
    }

    it = samples.find(hpc::Counter::GPUActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for GPUActiveCy");
        return 0.0;
    }

    auto value = (convertToDouble(samples[hpc::Counter::FragQueueActiveCy]) / convertToDouble(samples[hpc::Counter::GPUActiveCy])) * 100;
    return std::max(std::min(value, 100.0), 0.0);
}

double nonFragmentQueueUtilization(std::unordered_map<hpc::Counter, hpc::Sample> samples)
{
    auto it = samples.find(hpc::Counter::NonFragQueueActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for NonFragQueueActiveCy");
        return 0.0;
    }

    it = samples.find(hpc::Counter::GPUActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for GPUActiveCy");
        return 0.0;
    }

    auto value = (convertToDouble(samples[hpc::Counter::NonFragQueueActiveCy]) / convertToDouble(samples[hpc::Counter::GPUActiveCy])) * 100;
    return std::max(std::min(value, 100.0), 0.0);
}

double tilerUtilization(std::unordered_map<hpc::Counter, hpc::Sample> samples)
{
    auto it = samples.find(hpc::Counter::TilerActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for TilerActiveCy");
        return 0.0;
    }

    it = samples.find(hpc::Counter::GPUActiveCy);
    if (it == samples.end())
    {
        spdlog::debug("No sample value for GPUActiveCy");
        return 0.0;
    }

    auto value = (convertToDouble(samples[hpc::Counter::TilerActiveCy]) / convertToDouble(samples[hpc::Counter::GPUActiveCy])) * 100;
    return std::max(std::min(value, 100.0), 0.0);
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

        std::unordered_map<Counter, double> values = {
            { Counter::FragmentUtilization, fragmentQueueUtilization(sampleMap) },
            { Counter::NonFragmentUtilization, nonFragmentQueueUtilization(sampleMap) },
            { Counter::TilerUtilization, tilerUtilization(sampleMap) }
        };

        m_descriptor.listner(values);
    }

    m_time = currentTime;
}

} // namespace jipu