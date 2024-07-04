#pragma once

#include "hpc/gpu.h"
#include "hpc/instance.h"

#include "hpc/backend/instance.h"

#include <memory>

namespace hpc
{
namespace adreno
{

class AdrenoInstance final : public Instance
{
public:
    std::vector<std::unique_ptr<GPU>> gpus() override;

private:
    std::unique_ptr<hpc::device::Instance> m_instance = nullptr;
};

} // namespace adreno
} // namespace hpc