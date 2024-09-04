#pragma once

#include <atomic>

namespace jipu
{

class RefCounted
{

public:
    RefCounted();
    ~RefCounted() = default;

public:
    void addRef();
    void release();

private:
    std::atomic<int> m_count = 0;
};

} // namespace jipu