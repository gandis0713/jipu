#pragma once

#include <chrono>

namespace jipu
{

class HPCWatcher
{
public:
    HPCWatcher();

    void update();

private:
    uint32_t period = 1000; // 1 second
    std::chrono::milliseconds m_time = std::chrono::milliseconds::zero();
};

} // namespace jipu