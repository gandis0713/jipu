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
    JuneProcApiContextCreateBufferMemory ApiContextCreateBufferMemory = nullptr;
    JuneProcApiContextCreateTextureMemory ApiContextCreateTextureMemory = nullptr;
    JuneProcApiContextDestroy ApiContextDestroy = nullptr;
    JuneProcBufferMemoryCreateBuffer BufferMemoryCreateBuffer = nullptr;
    JuneProcBufferMemoryBeginAccess BufferMemoryBeginAccess = nullptr;
    JuneProcBufferMemoryEndAccess BufferMemoryEndAccess = nullptr;
    JuneProcBufferMemoryDestroy BufferMemoryDestroy = nullptr;
    JuneProcTextureMemoryCreateTexture TextureMemoryCreateTexture = nullptr;
    JuneProcTextureMemoryBeginAccess TextureMemoryBeginAccess = nullptr;
    JuneProcTextureMemoryEndAccess TextureMemoryEndAccess = nullptr;
    JuneProcTextureMemoryDestroy TextureMemoryDestroy = nullptr;
    JuneProcBufferDestroy BufferDestroy = nullptr;
    JuneProcBufferCreateFence BufferCreateFence = nullptr;
    JuneProcTextureCreateFence TextureCreateFence = nullptr;
    JuneProcTextureDestroy TextureDestroy = nullptr;
    JuneProcFenceDestroy FenceDestroy = nullptr;
};

} // namespace jipu