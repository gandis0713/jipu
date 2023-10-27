#pragma once

#include "window.h"
#include <filesystem>

namespace vkt
{

struct SampleDescriptor
{
    WindowDescriptor windowDescriptor;
    std::filesystem::path path;
};

class Sample : public Window
{
public:
    Sample() = delete;
    Sample(const SampleDescriptor& descriptor);
    virtual ~Sample();

protected:
    std::filesystem::path m_appPath;
    std::filesystem::path m_appDir;
};

} // namespace vkt
