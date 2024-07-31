#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "hpc/counter.h"
#include "hpc/sampler.h"

namespace jipu
{

using Values = std::vector<hpc::Sample>;
using Listner = std::function<void(Values)>;

struct HPCWatcherDescriptor
{
    std::unique_ptr<hpc::Sampler> sampler = nullptr;
    std::unordered_set<hpc::Counter> counters{};
    Listner listner{};
};

class HPCWatcher
{

public:
    HPCWatcher(HPCWatcherDescriptor descriptor);
    ~HPCWatcher();

    void start();
    void stop();
    void update();

private:
    HPCWatcherDescriptor m_descriptor{};

private:
    uint32_t period = 1000; // 1 second
    std::chrono::milliseconds m_time = std::chrono::milliseconds::zero();

    enum State
    {
        kStarted,
        kIsStopping,
        kStopped
    };

    std::thread m_thread{};
    std::atomic<State> m_state = State::kStopped;

public:
    using Ptr = std::unique_ptr<HPCWatcher>;
};

} // namespace jipu