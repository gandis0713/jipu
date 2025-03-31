#include "runner.h"

#include <chrono>

namespace jipu
{

Runner::Runner(uint32_t fps)
    : m_fps(fps)
    , m_running(false)
    , m_paused(false)
    , m_stop(false)
{
}

Runner::~Runner()
{
    if (m_running.load())
    {
        stop();
    }
}

void Runner::run(const std::function<void()>& beginCallback,
                 const std::function<void()>& loopCallback,
                 const std::function<void()>& endCallback)
{
    if (m_running.load())
    {
        return;
    }

    m_stop.store(false);
    m_paused.store(false);
    m_running.store(true);

    m_thread = std::thread([this, beginCallback, loopCallback, endCallback]() {
        beginCallback();
        auto frameDuration = std::chrono::milliseconds(1000 / m_fps);

        while (!m_stop.load())
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait(lock, [this]() { return !m_paused.load() || m_stop.load(); });
            }

            if (m_stop.load())
            {
                break;
            }

            auto startTime = std::chrono::steady_clock::now();

            loopCallback();

            auto elapsed = std::chrono::steady_clock::now() - startTime;
            auto sleepTime = frameDuration - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
            if (sleepTime > std::chrono::milliseconds(0))
            {
                std::this_thread::sleep_for(sleepTime);
            }
        }

        endCallback();
        m_running.store(false);
    });
}

void Runner::pause()
{
    m_paused.store(true);
}

void Runner::resume()
{
    m_paused.store(false);
    m_cv.notify_all();
}

void Runner::stop()
{
    m_stop.store(true);
    m_cv.notify_all();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

bool Runner::isRunning() const
{
    return m_running.load();
}
} // namespace jipu