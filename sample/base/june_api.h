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
    JuneProcApiContextDestroy ApiContextDestroy = nullptr;
    JuneProcSharedMemoryCreateBuffer SharedMemoryCreateBuffer = nullptr;
    JuneProcSharedMemoryCreateTexture SharedMemoryCreateTexture = nullptr;
    JuneProcSharedMemoryBeginAccess SharedMemoryBeginAccess = nullptr;
    JuneProcSharedMemoryEndAccess SharedMemoryEndAccess = nullptr;
    JuneProcSharedMemoryDestroy SharedMemoryDestroy = nullptr;
    JuneProcBufferDestroy BufferDestroy = nullptr;
    JuneProcBufferCreateFence BufferCreateFence = nullptr;
    JuneProcTextureCreateFence TextureCreateFence = nullptr;
    JuneProcTextureDestroy TextureDestroy = nullptr;
    JuneProcFenceDestroy FenceDestroy = nullptr;
};

} // namespace jipu