#include "june_api.h"

#include "jipu/common/dylib.h"

#include <spdlog/spdlog.h>

namespace jipu
{

bool JuneAPI::loadProcs(DyLib* juneLib)
{
#define GET_PROC(name)                                                     \
    name = reinterpret_cast<decltype(name)>(GetProcAddress("june" #name)); \
    if (name == nullptr)                                                   \
    {                                                                      \
        spdlog::error("Couldn't get proc june{}", #name);                  \
        return false;                                                      \
    }

    if (!juneLib->getProc(&GetProcAddress, "juneGetProcAddress"))
    {
        spdlog::error("Couldn't get juneGetProcAddress");
        return false;
    }

    GET_PROC(CreateInstance);
    GET_PROC(DestroyInstance);
    GET_PROC(CreateApiContext);
    GET_PROC(DestroyApiContext);
    GET_PROC(CreateBufferMemory);
    GET_PROC(DestroyBufferMemory);
    GET_PROC(CreateTextureMemory);
    GET_PROC(DestroyTextureMemory);
    GET_PROC(CreateBuffer);
    GET_PROC(DestroyBuffer);
    GET_PROC(CreateTexture);
    GET_PROC(DestroyTexture);

    return true;
}

} // namespace jipu