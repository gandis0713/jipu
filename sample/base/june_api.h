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

    JuneProcResourceBeginAccess ResourceBeginAccess = nullptr;
    JuneProcResourceEndAccess ResourceEndAccess = nullptr;
    JuneProcResourceConnect ResourceConnect = nullptr;
    JuneProcResourceGetResource ResourceGetResource = nullptr;
    JuneProcResourceDestroy ResourceDestroy = nullptr;

    JuneProcFenceDestroy FenceDestroy = nullptr;
};

} // namespace jipu