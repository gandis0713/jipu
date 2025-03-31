#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>

namespace jipu
{

class Runner
{
public:
    Runner() = delete;
    explicit Runner(uint32_t fps);
    ~Runner();

    Runner(const Runner&) = delete;
    Runner& operator=(const Runner&) = delete;

public:
    void run(const std::function<void()>& beginCallback,
             const std::function<void()>& loopCallback,
             const std::function<void()>& endCallback);

    void pause();
    void resume();
    void stop();
    bool isRunning() const;

private:
    uint32_t m_fps;
    std::thread m_thread;
    std::atomic<bool> m_running;
    std::atomic<bool> m_paused;
    std::atomic<bool> m_stop;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

} // namespace jipu