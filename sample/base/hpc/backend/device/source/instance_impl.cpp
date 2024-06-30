#include "instance_impl.h"

#include "handle_impl.h"
#include "syscall/interface.h"

#include <spdlog/spdlog.h>

#define ADRENO_IOCTL_TYPE 0x09

#define ADRENO_PROPERTY_DEVICE_INFO 0x1

struct adreno_device_info
{
    unsigned int device_id;
    unsigned int chip_id;
    unsigned int mmu_enabled;
    unsigned long gmem_gpu_base_address;
    unsigned int gpu_id;
    size_t gmem_sizebytes;
};

struct adreno_device_get_property
{
    unsigned int type;
    void* value;
    size_t num_bytes;
};

#define ADRENO_IOCTL_DEVICE_GET_PROPERTY \
    _IOWR(ADRENO_IOCTL_TYPE, 0x2, struct adreno_device_get_property)

struct adreno_counter_get
{
    unsigned int group_id;
    unsigned int countable_selector;
    unsigned int regster_offset_low;
    unsigned int regster_offset_high;
    unsigned int __pad;
};

#define ADRENO_IOCTL_COUNTER_GET \
    _IOWR(ADRENO_IOCTL_TYPE, 0x38, struct adreno_counter_get)

struct adreno_counter_put
{
    unsigned int group_id;
    unsigned int countable_selector;
    unsigned int __pad[2];
};

#define ADRENO_IOCTL_COUNTER_PUT \
    _IOW(ADRENO_IOCTL_TYPE, 0x39, struct adreno_counter_put)

struct adreno_counter_read
{
    struct hpc_gpu_adreno_ioctl_counter_read_counter_t* counters;
    unsigned int num_counters;
    unsigned int __pad[2];
};

#define ADRENO_IOCTL_COUNTER_READ \
    _IOWR(ADRENO_IOCTL_TYPE, 0x3B, struct adreno_counter_read)

/// Adreno GPU series.
enum class AdrenoSeries : uint32_t
{
    HPC_GPU_ADRENO_SERIES_UNKNOWN = 0,
    /// Adreno A6XX GPUs ((>= 600 && < 700) || == 702)
    HPC_GPU_ADRENO_SERIES_A6XX,
    /// Adreno A5XX GPUs (>= 500 && < 600)
    HPC_GPU_ADRENO_SERIES_A5XX,
};

AdrenoSeries getSeries(int gpu_id)
{
    if ((gpu_id >= 600 && gpu_id < 700) || gpu_id == 702)
        return AdrenoSeries::HPC_GPU_ADRENO_SERIES_A6XX;
    if (gpu_id >= 500 && gpu_id < 600)
        return AdrenoSeries::HPC_GPU_ADRENO_SERIES_A5XX;
    return AdrenoSeries::HPC_GPU_ADRENO_SERIES_UNKNOWN;
}

namespace hpc
{
namespace device
{

InstanceImpl::InstanceImpl(Handle& handle)
    : m_handle(handle)
{
    init();
}

void InstanceImpl::init()
{
    adreno_device_info devinfo{};

    adreno_device_get_property payload{};
    payload.type = ADRENO_PROPERTY_DEVICE_INFO;
    payload.value = &devinfo;
    payload.num_bytes = sizeof(adreno_device_info);

    auto& handleImpl = downcast(m_handle);
    auto result = syscall::Interface::ioctl(handleImpl.fd(), ADRENO_IOCTL_DEVICE_GET_PROPERTY, &payload);
    auto error = result.first;
    if (error)
    {
        // TODO: log
        return;
    }

    uint32_t chip_id = devinfo.chip_id;
    uint8_t core_id = (chip_id >> (8 * 3)) & 0xffu;
    uint8_t major_id = (chip_id >> (8 * 2)) & 0xffu;
    uint8_t minor_id = (chip_id >> (8 * 1)) & 0xffu;
    auto gpu_id = core_id * 100 + major_id * 10 + minor_id;

    spdlog::debug("charles chip_id: {}", chip_id);
    spdlog::debug("charles devinfo.device_id: {}", devinfo.device_id);
    spdlog::debug("charles gpu_id: {}", gpu_id);

    auto series = getSeries(gpu_id);

    spdlog::debug("charles series: {}", static_cast<uint32_t>(series));

    switch (series)
    {
    case AdrenoSeries::HPC_GPU_ADRENO_SERIES_A6XX: {
        for (int i = 0; i < context->num_counters; ++i)
        {
            int status = hpc_gpu_adreno_ioctl_activate_counter(
                context->gpu_device, context->counters[i].group_id,
                context->counters[i].countable_selector);
            if (status < 0)
                return status;
        }
    }
    break;
    case AdrenoSeries::HPC_GPU_ADRENO_SERIES_A5XX:

        break;
    default:
        // do nothing
        break;
    }
}

} // namespace device
} // namespace hpc