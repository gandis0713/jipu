#include "unique_handle.h"

#include <spdlog/spdlog.h>

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/sync.h>
#endif

namespace jipu
{

UniqueHandle UniqueHandle::merge(const char* name, std::vector<UniqueHandle>&& inHandles)
{
#if defined(__ANDROID__) || defined(ANDROID)
    inHandles.erase(std::remove_if(inHandles.begin(), inHandles.end(),
                                   [](const UniqueHandle& u) { return u.get() < 0; }),
                    inHandles.end());

    if (inHandles.empty())
    {
        spdlog::warn("No valid handles to merge");
        return UniqueHandle();
    }

    while (inHandles.size() > 1)
    {
        UniqueHandle a = std::move(inHandles.back());
        inHandles.pop_back();
        UniqueHandle b = std::move(inHandles.back());
        inHandles.pop_back();

        int merged = sync_merge(name, a.get(), b.get());
        if (merged < 0)
        {
            spdlog::warn("Failed to merge handles {} and {}", a.get(), b.get());
            continue;
        }

        inHandles.emplace_back(merged);
    }

    return std::move(inHandles.front());
#else
    throw std::runtime_error("sync_merge is not supported on this platform");

#endif
}

UniqueHandle::UniqueHandle()
#if defined(__ANDROID__) || defined(ANDROID)
    : m_handle(-1)
#elif defined(__APPLE__)
    : m_handle(-1)
#endif
{
}
UniqueHandle::UniqueHandle(HandleType handle)
    : m_handle(handle)
{
}

UniqueHandle::~UniqueHandle()
{
    clear();
}

UniqueHandle::UniqueHandle(UniqueHandle&& other)
    : m_handle(other.release())
{
}

UniqueHandle& UniqueHandle::operator=(UniqueHandle&& s)
{
    reset(s.release());
    return *this;
}

void UniqueHandle::reset(HandleType handle)
{
#if defined(__ANDROID__) || defined(ANDROID)
    if (m_handle != -1)
    {
        spdlog::trace("close handle {}", m_handle);
        close(m_handle);
    }
#elif defined(__APPLE__)
    if (m_handle != -1)
        close(m_handle);
#endif
    m_handle = handle;
}

void UniqueHandle::clear()
{
#if defined(__ANDROID__) || defined(ANDROID)
    HandleType handle = -1;
#elif defined(__APPLE__)
    HandleType handle = -1;
#endif
    reset(handle);
}

HandleType UniqueHandle::get() const
{
    return m_handle;
}

HandleType UniqueHandle::release()
{
    HandleType ret = m_handle;
#if defined(__ANDROID__) || defined(ANDROID)
    m_handle = -1;
#elif defined(__APPLE__)
    m_handle = -1;
#endif
    return ret;
}

} // namespace jipu