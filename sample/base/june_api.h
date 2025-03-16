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
    JuneProcDestroyInstance DestroyInstance = nullptr;
    JuneProcCreateApiContext CreateApiContext = nullptr;
    JuneProcDestroyApiContext DestroyApiContext = nullptr;
    JuneProcCreateBufferMemory CreateBufferMemory = nullptr;
    JuneProcDestroyBufferMemory DestroyBufferMemory = nullptr;
    JuneProcCreateTextureMemory CreateTextureMemory = nullptr;
    JuneProcDestroyTextureMemory DestroyTextureMemory = nullptr;
    JuneProcCreateBuffer CreateBuffer = nullptr;
    JuneProcDestroyBuffer DestroyBuffer = nullptr;
    JuneProcCreateTexture CreateTexture = nullptr;
    JuneProcDestroyTexture DestroyTexture = nullptr;
};

} // namespace jipu