#include "adreno_sampler.h"

#include "ioctl/a6xx.h"
#include "ioctl/api.h"
#include "ioctl/utils.h"
#include "syscall/interface.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace backend
{
namespace adreno
{

AdrenoSampler::AdrenoSampler(const AdrenoGPU& gpu, std::unique_ptr<Handle> handle, const SamplerDescriptor& descriptor)
    : m_gpu(gpu)
    , m_handle(std::move(handle))
    , m_descriptor(descriptor)
{
}

std::error_code AdrenoSampler::start()
{
    return activate();
}

std::error_code AdrenoSampler::stop()
{
    return deactivate();
}

std::unordered_map<Counter, Sample> AdrenoSampler::sample(std::unordered_set<Counter> counters)
{
    std::vector<AdrenoIoctlCounterReadCounter> counterReadCounters{};
    for (const auto counter : counters)
    {
        AdrenoIoctlCounterReadCounter counterReadCounter{};
        counterReadCounter.group_id = getGroup(counter);
        counterReadCounter.countable_selector = getSelector(counter);

        counterReadCounters.push_back(counterReadCounter);
    }

    AdrenoCounterRead counterRead{};
    counterRead.num_counters = static_cast<unsigned int>(counterReadCounters.size());
    counterRead.counters = reinterpret_cast<AdrenoIoctlCounterReadCounter*>(counterReadCounters.data());

    auto result = syscall::Interface::ioctl(m_handle->fd(), ADRENO_IOCTL_COUNTER_READ, &counterRead);
    auto error = result.first;
    if (error)
    {
        spdlog::error("Failed to sampling counters for adreno, error: {} {}", error.value(), error.message());
        return {};
    }

    std::unordered_map<Counter, Sample> sampledValues{};
    for (const auto counterReadCounter : counterReadCounters)
    {
        spdlog::error("counter: {}, selector:{}, value {}", getCounter(counterReadCounter.group_id, counterReadCounter.countable_selector), counterReadCounter.countable_selector, counterReadCounter.value);
        sampledValues.insert({ getCounter(counterReadCounter.group_id, counterReadCounter.countable_selector), counterReadCounter.value });
    }

    return sampledValues;
}

std::error_code AdrenoSampler::activate()
{
    for (const auto counter : m_descriptor.counters)
    {
        AdrenoCounterGet counterGet{};
        counterGet.group_id = getGroup(counter);
        counterGet.countable_selector = getSelector(counter);

        auto result = syscall::Interface::ioctl(m_handle->fd(), ADRENO_IOCTL_COUNTER_GET, &counterGet);

        auto error = result.first;
        if (error)
        {
            spdlog::error("Failed to activate counters for adreno counter {}, error: {} {}", static_cast<uint32_t>(counter), error.value(), error.message());
            return error;
        }
    }

    // check activated counters
    {
        std::unordered_set<uint32_t> groupIds{};
        for (const auto& counter : m_descriptor.counters)
        {
            groupIds.insert(getGroup(counter));
        }

        for (const auto& groupId : groupIds)
        {
            AdrenoPerfcounterQuery counterQuery{};
            counterQuery.groupid = groupId;
            auto result = syscall::Interface::ioctl(m_handle->fd(), ADRENO_IOCTL_PERFCOUNTER_QUERY, &counterQuery);

            if (result.first)
            {
                spdlog::debug("Failed to query activated performance count value. error: {}, msg: {}", result.first.value(), result.first.message());
                continue;
            }

            unsigned int initValue = std::numeric_limits<unsigned int>::max();
            std::vector<unsigned int> countables(counterQuery.max_counters, initValue);
            counterQuery.countables = countables.data();
            counterQuery.count = static_cast<unsigned int>(countables.size());

            result = syscall::Interface::ioctl(m_handle->fd(), ADRENO_IOCTL_PERFCOUNTER_QUERY, &counterQuery);
            if (result.first)
            {
                spdlog::debug("Failed to query activated performance counters. error: {}, msg: {}", result.first.value(), result.first.message());
                continue;
            }

            spdlog::debug("Activated Performance Counter. groupid: {}, count: {}, max counters: {}", counterQuery.groupid, counterQuery.count, counterQuery.max_counters);
            for (auto i = 0; i < counterQuery.count; ++i)
            {
                spdlog::debug("    [{}] countables: {}", i, counterQuery.countables[i]);
            }
        }
    }

    return {};
}

std::error_code AdrenoSampler::deactivate()
{
    for (const auto counter : m_descriptor.counters)
    {
        AdrenoCounterPut counterPut{};
        counterPut.group_id = getGroup(counter);
        counterPut.countable_selector = getSelector(counter);

        auto result = syscall::Interface::ioctl(m_handle->fd(), ADRENO_IOCTL_COUNTER_PUT, &counterPut);

        auto error = result.first;
        if (error)
        {
            spdlog::error("Failed to deactivate counters for adreno counter {}, error: {} {}", static_cast<uint32_t>(counter), error.value(), error.message());
            return error;
        }
    }

    return {};
}

} // namespace adreno
} // namespace backend
} // namespace hpc