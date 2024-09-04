#pragma once

#include "window.h"

#include <filesystem>
#include <webgpu.h>

namespace jipu
{

struct WGPUSampleDescriptor
{
    WindowDescriptor windowDescriptor;
    std::filesystem::path path;
};

class WGPUSample : public Window
{
public:
    WGPUSample() = delete;
    WGPUSample(const WGPUSampleDescriptor& descriptor);
    virtual ~WGPUSample();

public:
    void init() override;
    void update() override;

protected:
    std::filesystem::path m_appPath;
    std::filesystem::path m_appDir;
};

} // namespace jipu
