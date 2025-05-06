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
    JuneProcInstanceImportSharedMemory InstanceImportSharedMemory = nullptr;
    JuneProcInstanceDestroy InstanceDestroy = nullptr;

    JuneProcApiContextCreateResource ApiContextCreateResource = nullptr;
    JuneProcApiContextCreateFence ApiContextCreateFence = nullptr;
    JuneProcApiContextDestroy ApiContextDestroy = nullptr;

    JuneProcSharedMemoryDestroy SharedMemoryDestroy = nullptr;

    JuneProcFenceReset FenceReset = nullptr;
    JuneProcFenceExport FenceExport = nullptr;
    JuneProcFenceDestroy FenceDestroy = nullptr;
};

} // namespace jipu