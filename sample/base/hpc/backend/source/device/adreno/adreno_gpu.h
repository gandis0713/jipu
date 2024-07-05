#pragma once

#include "hpc/backend/gpu.h"

#include <string>

namespace hpc
{
namespace backend
{
namespace adreno
{

class AdrenoGPU final : public GPU
{
public:
    explicit AdrenoGPU(const std::string& path);
    ~AdrenoGPU() override = default;

    std::unique_ptr<Sampler> createSampler() override;

private:
    /**
     * @brief Path to the device file descriptor.
     */
    std::string m_path{};
};

} // namespace adreno
} // namespace backend
} // namespace hpc