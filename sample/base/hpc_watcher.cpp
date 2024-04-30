#include "hpc_watcher.h"

#include <spdlog/spdlog.h>

using namespace std::chrono;

namespace jipu
{

HPCWatcher::HPCWatcher(hpc::Sampler::Ptr sampler, Listner listner)
    : m_sampler(std::move(sampler))
    , m_listner(listner)
    , m_time(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()))
{
}

void HPCWatcher::start()
{
    m_sampler->start();
}

void HPCWatcher::stop()
{
    m_sampler->stop();
}

void HPCWatcher::update()
{
    auto currentTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_time).count();

    if (durationTime < period)
        return;

    m_listner(m_sampler->samples());

    m_time = currentTime;
}

} // namespace jipu