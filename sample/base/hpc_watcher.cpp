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

HPCWatcher::~HPCWatcher()
{
    stop();
}

void HPCWatcher::start()
{
    if (m_state.load() != State::kStopped)
    {
        return;
    }

    m_state.store(State::kStarted);
    m_thread = std::thread([this]() {
        m_descriptor.sampler->start();
        while (m_state.load() == State::kStarted)
        {
            update();
            std::this_thread::sleep_for(std::chrono::milliseconds(period));
        }
        m_state.store(State::kStopped);
    });

    m_thread.detach();
}

void HPCWatcher::stop()
{
    if (m_state.load() != State::kStarted)
    {
        return;
    }

    m_state.store(State::kIsStopping);

    while (m_state.load() != State::kStopped)
    {
        // wait...
    }

    m_descriptor.sampler->stop();
}

void HPCWatcher::update()
{
    auto currentTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_time).count();

    if (durationTime < period)
        return;

    m_descriptor.listner(m_descriptor.sampler->samples());

    m_time = currentTime;
}

} // namespace jipu