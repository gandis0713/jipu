#include "hpc_watcher.h"

#include <spdlog/spdlog.h>

using namespace std::chrono;

namespace jipu
{

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
        std::vector<hpc::Sample> values{};
        for (const auto& sample : samples)
        {
            values.push_back(sample);
        }

        m_descriptor.listner(values);

        m_time = currentTime;
    }
}

} // namespace jipu