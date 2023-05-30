#pragma once

#include "window.h"
#include <filesystem>

class Sample : public Window
{
public:
    Sample() = delete;
    Sample(int width, int height, const std::string& title, const char* path);
    virtual ~Sample() = default;

protected:
    std::filesystem::path m_path;
};
