#include "mali_instance.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace backend
{
namespace mali {

    std::unique_ptr <Instance> MaliInstance::create() {
        auto handle = hpc::backend::Handle::create("/dev/mali0");
        if (!handle) {
            spdlog::error("Failed to create mali device handle");
            return nullptr;
        }

        return std::make_unique<MaliInstance>(std::move(handle));
    }

    MaliInstance::MaliInstance(std::unique_ptr <Handle> handle)
            : m_handle(std::move(handle)) {
    }

} // namespace mali
} // namespace backend
} // namespace hpc