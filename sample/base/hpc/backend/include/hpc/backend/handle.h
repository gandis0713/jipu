#pragma once

#include "export.h"

#include <memory>

namespace hpc
{
namespace backend
{

class HPC_DEVICE_VISIBILITY Handle
{
public:
    /**
     * @brief Create a new device handle for gpu counter.
     * @param path The path to the device file descriptor.
     */
    static std::unique_ptr<Handle> create(const char* path);

    virtual ~Handle() = default;

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

protected:
    Handle() = default;
};

} // namespace backend
} // namespace hpc