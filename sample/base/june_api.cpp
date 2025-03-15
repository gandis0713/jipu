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

    return true;
}

} // namespace jipu