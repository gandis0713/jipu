#include "hpc_watcher.h"

#include <spdlog/spdlog.h>

using namespace std::chrono;

namespace jipu
{

HPCWatcher::HPCWatcher(hpc::Sampler::Ptr sampler)
    : m_sampler(std::move(sampler))
    , m_time(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()))
{
    spdlog::error("charles {}", __func__);
}

void HPCWatcher::update()
{
    auto currentTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_time).count();

    if (durationTime < period)
        return;

    m_time = currentTime;
}

} // namespace jipu