#pragma once

#include <cstdint>

namespace hpc
{
namespace backend
{
namespace ioctl
{
namespace mali
{

struct VersionCheck
{
    uint16_t major;
    uint16_t minor;
};

} // namespace mali
} // namespace ioctl
} // namespace backend
} // namespace hpc