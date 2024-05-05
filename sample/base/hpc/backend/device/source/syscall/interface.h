#pragma once

#include <errno.h>
#include <sys/stat.h>
#include <system_error>

#include <utility>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace hpc
{
namespace device
{
namespace syscall
{

class Interface
{

public:
    static std::pair<std::error_code, int> open(const char* name, int oflags)
    {
        const int fd = ::open(name, oflags);

        std::error_code ec;
        if (fd < 0)
            ec = errorCode();

        return std::make_pair(ec, fd);
    }

    static std::error_code close(int fd)
    {
        const int result = ::close(fd);

        std::error_code ec;
        if (result < 0)
            ec = errorCode();

        return ec;
    }

    template <typename command_t, typename... args_t>
    static std::pair<std::error_code, int> ioctl(int fd, command_t command, args_t&&... args)
    {
        const int result = ::ioctl(fd, command, std::forward<args_t>(args)...);

        std::error_code ec;
        if (result < 0)
            ec = errorCode();

        return std::make_pair(ec, result);
    }

private:
    static std::error_code errorCode()
    {
        return { errno, std::generic_category() };
    }
};

} // namespace syscall
} // namespace device
} // namespace hpc