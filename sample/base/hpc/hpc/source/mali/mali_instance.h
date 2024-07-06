#pragma once

#include "hpc/gpu.h"
#include "hpc/instance.h"

namespace hpc
{
namespace mali
{

class MaliInstance final : public Instance
{
public:
    std::vector<std::unique_ptr<hpc::GPU>> gpus() override;
};

} // namespace mali
} // namespace hpc