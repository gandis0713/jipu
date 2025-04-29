#pragma once

#include "june/june.h"

namespace jipu
{

class DyLib;
struct JuneAPI
{
    bool loadProcs(DyLib* JuneLib);

    JuneProcGetProcAddress GetProcAddress = nullptr;
    JuneProcCreateInstance CreateInstance = nullptr;

    JuneProcInstanceCreateApiContext InstanceCreateApiContext = nullptr;
    JuneProcInstanceCreateSharedMemory InstanceCreateSharedMemory = nullptr;
    JuneProcInstanceDestroy InstanceDestroy = nullptr;

    JuneProcApiContextCreateResource ApiContextCreateResource = nullptr;
    JuneProcApiContextCreateFence ApiContextCreateFence = nullptr;
    JuneProcApiContextDestroy ApiContextDestroy = nullptr;

    JuneProcSharedMemoryBeginAccess SharedMemoryBeginAccess = nullptr;
    JuneProcSharedMemoryEndAccess SharedMemoryEndAccess = nullptr;
    JuneProcSharedMemoryDestroy SharedMemoryDestroy = nullptr;

    JuneProcFenceDestroy FenceDestroy = nullptr;
};

} // namespace jipu