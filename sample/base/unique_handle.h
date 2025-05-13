#pragma once

#if defined(__ANDROID__) || defined(ANDROID)
#include <unistd.h>
using HandleType = int;
#elif defined(__APPLE__)
#include <unistd.h>
using HandleType = int;
#endif

#include <vector>

namespace jipu
{

class UniqueHandle final
{
public:
    static UniqueHandle merge(const char* name, std::vector<UniqueHandle>&& inHandles);

public:
    UniqueHandle();
    explicit UniqueHandle(HandleType handle);
    ~UniqueHandle();
    UniqueHandle(UniqueHandle&& other);
    UniqueHandle& operator=(UniqueHandle&& s);

    void reset(HandleType handle);
    void clear();
    HandleType get() const;
    HandleType release();

private:
    HandleType m_handle;
};

} // namespace jipu