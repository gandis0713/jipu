#pragma once

#include "hpc/gpu.h"

namespace hpc
{
namespace mali
{

std::vector<std::unique_ptr<GPU>> gpus();

}
} // namespace hpc