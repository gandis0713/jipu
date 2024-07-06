#pragma once

#include "hpc/backend/gpu.h"
#include "hpc/backend/instance.h"

#include <string>

namespace hpc
{
namespace backend
{

namespace adreno
{

class AdrenoInstance;
class AdrenoGPU final : public GPU
{
public:
    explicit AdrenoGPU(AdrenoInstance& instance, const std::string& path);
    ~AdrenoGPU() override = default;

    Instance& getInstance() override;
    std::unique_ptr<Sampler> createSampler() override;

private:
    AdrenoInstance& m_instance;

private:
    /**
     * @brief Path to the device file descriptor.
     */
    std::string m_path{};
};

} // namespace adreno
} // namespace backend
} // namespace hpc