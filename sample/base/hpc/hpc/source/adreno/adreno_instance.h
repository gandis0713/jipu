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
    static std::unique_ptr<hpc::Instance> create();
    explicit AdrenoInstance(std::unique_ptr<hpc::backend::Instance> instance);

public:
    std::vector<std::unique_ptr<hpc::GPU>> gpus() override;

private:
    std::unique_ptr<hpc::backend::Instance> m_instance = nullptr;
};

} // namespace adreno
} // namespace hpc