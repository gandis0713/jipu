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

    JuneProcApiContextCreateApiMemory ApiContextCreateApiMemory = nullptr;
    JuneProcApiContextCreateFence ApiContextCreateFence = nullptr;
    JuneProcApiContextDestroy ApiContextDestroy = nullptr;

    JuneProcApiMemoryBeginAccess ApiMemoryBeginAccess = nullptr;
    JuneProcApiMemoryEndAccess ApiMemoryEndAccess = nullptr;
    JuneProcApiMemoryCreateResource ApiMemoryCreateResource = nullptr;
    JuneProcApiMemoryDestroy ApiMemoryDestroy = nullptr;

    JuneProcFenceDestroy FenceDestroy = nullptr;
};

} // namespace jipu