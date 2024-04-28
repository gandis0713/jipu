#pragma once

#include "gpu.h"

namespace jipu
{
namespace hpc
{

class Sampler
{
public:
    explicit Sampler(GPU::Ptr gpu);

private:
    GPU::Ptr m_gpu{ nullptr };
};

} // namespace hpc
} // namespace jipu