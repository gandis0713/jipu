#pragma once

#include <stdint.h>

namespace jipu
{
namespace hpc
{

class Handle
{
private:
    int m_fd{ -1 };
};

} // namespace hpc
} // namespace jipu