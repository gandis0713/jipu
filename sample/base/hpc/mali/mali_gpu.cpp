#include "mali_gpu.h"

#include "mali_counter.h"

#include <hwcpipe/gpu.hpp>
#include <spdlog/spdlog.h>

namespace jipu
{
namespace hpc
{
namespace mali
{

MaliGPU::MaliGPU(int deviceNumber)
    : id{ 0 } // TODO
    , m_deviceNumber(deviceNumber){};

Counter::Ptr MaliGPU::create()
{
    return std::make_unique<MaliCounter>(*this);
}

} // namespace mali
} // namespace hpc
} // namespace jipu