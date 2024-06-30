#pragma once

#include "hpc/gpu.h"
#include "hpc/instance.h"

namespace hpc
{
namespace adreno
{

class AdrenoInstance final : public Instance
{
public:
    std::vector<std::unique_ptr<GPU>> gpus() override;
};

} // namespace adreno
} // namespace hpc