#include "hpc_watcher.h"

using namespace std::chrono;

namespace jipu
{

HPCWatcher::HPCWatcher()
    : m_time(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()))
{
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