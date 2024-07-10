#include "adreno_sampler.h"

#include "ioctl/a6xx.h"
#include "ioctl/api.h"
#include "ioctl/utils.h"
#include "syscall/interface.h"

#include <spdlog/spdlog.h>
#include <thread>

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
    auto t = std::thread([&]() {
        activeCounters();

        auto& counters = m_descriptor.counters;
        auto& handle = *m_handle;

        auto groupId = getGroup(*counters.begin());
        AdrenoPerfcounterQuery counterQuery{};
        counterQuery.groupid = groupId;
        auto result = syscall::Interface::ioctl(handle.fd(), ADRENO_IOCTL_PERFCOUNTER_QUERY, &counterQuery);

        if (result.first)
        {
            spdlog::debug("charles error.code {}", result.first.value());
            spdlog::debug("charles error.message {}", result.first.message());
            spdlog::debug("charles result {}", result.second);
        }

        spdlog::debug("charles counterQuery.groupid {}", counterQuery.groupid);
        spdlog::debug("charles counterQuery.count {}", counterQuery.count);
        spdlog::debug("charles counterQuery.max_counters {}", counterQuery.max_counters);

        for (auto i = 0; i < counterQuery.count; ++i)
        {
            spdlog::debug("charles counterQuery.countables[{}] {}", i, counterQuery.countables[i]);
        }

        while (true)
        {
            std::vector<AdrenoIoctlCounterReadCounter> counterReadValues{};
            for (const auto counter : counters)
            {
                AdrenoIoctlCounterReadCounter counterReadValue{};
                counterReadValue.group_id = getGroup(counter);
                counterReadValue.countable_selector = getSelector(counter);

                counterReadValues.push_back(counterReadValue);
            }

            AdrenoCounterRead counterRead{};
            counterRead.num_counters = static_cast<unsigned int>(counterReadValues.size());
            counterRead.counters = reinterpret_cast<AdrenoIoctlCounterReadCounter*>(counterReadValues.data());

            auto result = syscall::Interface::ioctl(handle.fd(), ADRENO_IOCTL_COUNTER_READ, &counterRead);

            spdlog::debug("charles error.code {}", result.first.value());
            spdlog::debug("charles error.message {}", result.first.message());
            spdlog::debug("charles result {}", result.second);

            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (result.first)
            {
                // TODO: log error
                continue;
            }

            for (int i = 0; i < counterReadValues.size(); ++i)
            {
                spdlog::debug("charles counter {} value {}", static_cast<uint64_t>(counterReadValues[i].countable_selector), counterReadValues[i].value);
            }
        }
    });
    t.detach();

    return {};
}

std::error_code AdrenoSampler::activeCounters()
{
    for (const auto counter : m_descriptor.counters)
    {
        auto groupId = getGroup(counter);
        auto countableSelector = getSelector(counter);

        AdrenoCounterGet counterGet{};
        counterGet.group_id = groupId;
        counterGet.countable_selector = countableSelector;
        auto result = syscall::Interface::ioctl(m_handle->fd(), ADRENO_IOCTL_COUNTER_GET, &counterGet);
        auto error = result.first;

        if (error)
        {
            spdlog::error("Failed to active counters for adreno counter {}, error: {} {}", static_cast<uint32_t>(counter), error.value(), error.message());
            return error;
        }
    }

    return {};
}

std::error_code AdrenoSampler::stop()
{
    return {};
}

} // namespace adreno
} // namespace backend
} // namespace hpc