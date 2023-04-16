#pragma once

#include "utils/assert.h"

#include <string>
#include <type_traits>

namespace vkt
{

class DynamicLib
{
public:
    DynamicLib() = default;
    ~DynamicLib();

    DynamicLib(const DynamicLib&) = delete;
    DynamicLib& operator=(const DynamicLib&) = delete;

    DynamicLib(DynamicLib&& other);
    DynamicLib& operator=(DynamicLib&& other);

    bool isValid() const;

    bool open(const std::string& filename);
    void close();

    void* getProc(const std::string& procName) const;

    template <typename T>
    bool getProc(T** proc, const std::string& procName) const
    {
        assert_message(proc != nullptr, "proc is null pointer.");
        static_assert(std::is_function<T>::value, "");

        *proc = reinterpret_cast<T*>(getProc(procName));
        return *proc != nullptr;
    }

private:
    void* m_handle{ nullptr };
};
} // namespace vkt