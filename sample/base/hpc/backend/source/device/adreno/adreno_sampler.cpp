#include "adreno_sampler.h"

#include "ioctl/a6xx.h"
#include "ioctl/types.h"
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

AdrenoSampler::AdrenoSampler(const AdrenoGPU& gpu, std::unique_ptr<Handle> handle)
    : m_gpu(gpu)
    , m_handle(std::move(handle))
{
}

std::error_code AdrenoSampler::start()
{
    auto t = std::thread([&]() {
        auto& counters = m_counters;
        auto& handle = *m_handle;

        for (const auto counter : counters)
        {
            auto groupId = getGroup(counter);
            auto countableSelector = getSelector(counter);

            adreno_counter_get counterGet{};
            counterGet.group_id = groupId;
            counterGet.countable_selector = countableSelector;
            auto result = syscall::Interface::ioctl(handle.fd(), ADRENO_IOCTL_COUNTER_GET, &counterGet);

            spdlog::debug("charles groupid {}", groupId);
            spdlog::debug("charles countableSelector {}", countableSelector);
            spdlog::debug("charles error.code {}", result.first.value());
            spdlog::debug("charles error.message {}", result.first.message());
            spdlog::debug("charles result {}", result.second);
            if (result.first)
            {
                return;
            }
        }

        auto groupId = getGroup(*counters.begin());
        adreno_perfcounter_query counterQuery{};
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
            std::vector<hpc_gpu_adreno_ioctl_counter_read_counter_t> counterReadValues{};
            for (const auto counter : counters)
            {
                hpc_gpu_adreno_ioctl_counter_read_counter_t counterReadValue{};
                counterReadValue.group_id = getGroup(counter);
                counterReadValue.countable_selector = getSelector(counter);

                counterReadValues.push_back(counterReadValue);
            }

            adreno_counter_read counterRead{};
            counterRead.num_counters = static_cast<unsigned int>(counterReadValues.size());
            counterRead.counters = reinterpret_cast<hpc_gpu_adreno_ioctl_counter_read_counter_t*>(counterReadValues.data());

            result = syscall::Interface::ioctl(handle.fd(), ADRENO_IOCTL_COUNTER_READ, &counterRead);

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
std::error_code AdrenoSampler::stop()
{
    return {};
}

void AdrenoSampler::setCounters(const std::unordered_set<uint32_t>& counters)
{
    m_counters = counters;
}

} // namespace adreno
} // namespace backend
} // namespace hpc