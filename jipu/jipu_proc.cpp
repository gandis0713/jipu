#include "webgpu.h"

#include "jipu/instance.h"
#include "jipu/physical_device.h"

#include <unordered_map>

namespace jipu
{

WGPUInstance procCreateInstance(WGPUInstanceDescriptor const* wgpuDescriptor)
{
    // auto descriptor = reinterpret_cast<InstanceDescriptor const*>(wgpuDescriptor);
    // auto result = Instance::create(descriptor);
    // return reinterpret_cast<WGPUInstanceImpl*>(result.release());
    return nullptr;
}

namespace
{
std::unordered_map<const char*, WGPUProc> sProcMap{
    { "wgpuCreateInstance", reinterpret_cast<WGPUProc>(procCreateInstance) },
};

} // namespace

WGPUProc procGetProcAddress(WGPUDevice, char const* procName)
{
    if (procName == nullptr)
    {
        return nullptr;
    }

    if (sProcMap.contains(procName))
    {
        return sProcMap[procName];
    }

    return nullptr;
}

} // namespace jipu