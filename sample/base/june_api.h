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
    JuneProcInstanceDestroy InstanceDestroy = nullptr;
    JuneProcApiContextCreateSharedMemory ApiContextCreateSharedMemory = nullptr;
    JuneProcApiContextCreateBuffer ApiContextCreateBuffer = nullptr;
    JuneProcApiContextCreateTexture ApiContextCreateTexture = nullptr;
    JuneProcApiContextDestroy ApiContextDestroy = nullptr;
    JuneProcSharedMemoryBeginAccess SharedMemoryBeginAccess = nullptr;
    JuneProcSharedMemoryEndAccess SharedMemoryEndAccess = nullptr;
    JuneProcSharedMemoryDestroy SharedMemoryDestroy = nullptr;
    JuneProcBufferCreateFence BufferCreateFence = nullptr;
    JuneProcBufferGetVkBuffer BufferGetVkBuffer = nullptr;
    JuneProcBufferDestroy BufferDestroy = nullptr;
    JuneProcTextureCreateFence TextureCreateFence = nullptr;
    JuneProcTextureGetVkImage TextureGetVkImage = nullptr;
    JuneProcTextureDestroy TextureDestroy = nullptr;
    JuneProcFenceDestroy FenceDestroy = nullptr;
};

} // namespace jipu