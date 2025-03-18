#include "june_api.h"

#include "jipu/common/dylib.h"

#include <spdlog/spdlog.h>

namespace jipu
{

bool JuneAPI::loadProcs(DyLib* juneLib)
{
#define GET_PROC(name)                                                             \
    std::string name##Str = "june" #name;                                          \
    StringView name##View{ .data = name##Str.data(), .length = name##Str.size() }; \
    name = reinterpret_cast<decltype(name)>(GetProcAddress(name##View));           \
    if (name == nullptr)                                                           \
    {                                                                              \
        spdlog::error("Couldn't get proc june{}", #name);                          \
        return false;                                                              \
    }

    if (!juneLib->getProc(&GetProcAddress, "juneGetProcAddress"))
    {
        spdlog::error("Couldn't get juneGetProcAddress");
        return false;
    }

    GET_PROC(CreateInstance);
    GET_PROC(InstanceCreateApiContext);
    GET_PROC(InstanceDestroy);
    GET_PROC(ApiContextCreateBufferMemory);
    GET_PROC(ApiContextCreateTextureMemory);
    GET_PROC(ApiContextDestroy);
    GET_PROC(BufferMemoryCreateBuffer);
    GET_PROC(BufferMemoryBeginAccess);
    GET_PROC(BufferMemoryEndAccess);
    GET_PROC(BufferMemoryDestroy);
    GET_PROC(TextureMemoryCreateTexture);
    GET_PROC(TextureMemoryBeginAccess);
    GET_PROC(TextureMemoryEndAccess);
    GET_PROC(TextureMemoryDestroy);
    GET_PROC(BufferDestroy);
    GET_PROC(BufferCreateFence);
    GET_PROC(TextureCreateFence);
    GET_PROC(TextureDestroy);
    GET_PROC(FenceDestroy);

    return true;
}

} // namespace jipu