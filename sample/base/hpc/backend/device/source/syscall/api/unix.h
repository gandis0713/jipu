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
    template <typename... Args>
    static int open(Args&&... args)
    {
        return ::open(std::forward<Args>(args)...);
    }
    template <typename... Args>
    static int close(Args&&... args)
    {
        return ::close(std::forward<Args>(args)...);
    }
    template <typename... Args>
    static int ioctl(Args&&... args)
    {
        return ::ioctl(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void* mmap(Args&&... args)
    {
        return ::mmap(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static int munmap(Args&&... args)
    {
        return ::munmap(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static int poll(Args&&... args)
    {
        return ::poll(std::forward<Args>(args)...);
    }
};

} // namespace api
} // namespace syscall
} // namespace device
} // namespace hpc