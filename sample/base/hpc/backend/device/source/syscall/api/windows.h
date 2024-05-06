#pragma once

#include <utility>

namespace hpc
{
namespace device
{
namespace syscall
{
namespace api
{

class Windows
{
public:
    template <typename... args_t>
    static int open(args_t&&... args)
    {
        return -1;
    }
    template <typename... args_t>
    static int close(args_t&&... args)
    {
        return -1;
    }
    template <typename... args_t>
    static int ioctl(args_t&&... args)
    {
        return -1;
    }

    template <typename... args_t>
    static void* mmap(args_t&&... args)
    {
        return nullptr;
    }

    template <typename... args_t>
    static int munmap(args_t&&... args)
    {
        return -1;
    }

    template <typename... args_t>
    static int poll(args_t&&... args)
    {
        return -1;
    }
};

} // namespace api
} // namespace syscall
} // namespace device
} // namespace hpc