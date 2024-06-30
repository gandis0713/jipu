#pragma once

#include <memory>

namespace hpc
{
namespace device
{

class Handle
{
public:
    /**
     * @brief Create a new device handle for gpu counter.
     * @param path The path to the device file descriptor.
     */
    static std::unique_ptr<Handle> create(const char* path);

    Handle() = default;
    virtual ~Handle() = default;

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
};

} // namespace device
} // namespace hpc