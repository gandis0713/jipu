#pragma once

#include <atomic>

namespace jipu
{

class Ref
{

public:
    Ref();
    ~Ref() = default;

public:
    void increase();
    bool decrease();
    void release();

private:
    std::atomic<int> m_count = 0;
};

} // namespace jipu