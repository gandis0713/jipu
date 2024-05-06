#pragma once

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <utility>

namespace hpc
{
namespace device
{
namespace syscall
{
namespace api
{

class Unix
{
public:
    template <typename... args_t>
    static int open(args_t&&... args)
    {
        return ::open(std::forward<args_t>(args)...);
    }
    template <typename... args_t>
    static int close(args_t&&... args)
    {
        return ::close(std::forward<args_t>(args)...);
    }
    template <typename... args_t>
    static int ioctl(args_t&&... args)
    {
        return ::ioctl(std::forward<args_t>(args)...);
    }

    template <typename... args_t>
    static void* mmap(args_t&&... args)
    {
        return ::mmap(std::forward<args_t>(args)...);
    }

    template <typename... args_t>
    static int munmap(args_t&&... args)
    {
        return ::munmap(std::forward<args_t>(args)...);
    }

    template <typename... args_t>
    static int poll(args_t&&... args)
    {
        return ::poll(std::forward<args_t>(args)...);
    }
};

} // namespace api
} // namespace syscall
} // namespace device
} // namespace hpc