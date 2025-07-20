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
    JuneProcInstanceCreateFence InstanceCreateFence = nullptr;
    JuneProcInstanceDestroy InstanceDestroy = nullptr;

    JuneProcApiContextCreateResource ApiContextCreateResource = nullptr;
    JuneProcApiContextExportFence ApiContextExportFence = nullptr;
    JuneProcApiContextDestroy ApiContextDestroy = nullptr;

    JuneProcSharedMemoryDestroy SharedMemoryDestroy = nullptr;

    JuneProcFenceReset FenceReset = nullptr;
    JuneProcFenceDestroy FenceDestroy = nullptr;
};

} // namespace jipu