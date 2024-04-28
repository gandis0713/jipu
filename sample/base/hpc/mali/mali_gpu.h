#pragma once

#include "gpu.h"

namespace jipu
{
namespace hpc
{

class MaliGPU final : public GPU
{
private:
    int m_deviceNumber{ -1 };
};

} // namespace hpc
} // namespace jipu