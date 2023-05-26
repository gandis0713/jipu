#pragma once

#include "vkt/gpu/buffer.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/queue.h"
#include "vkt/gpu/shader_module.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"
#include "vkt/gpu/texture_view.h"

#include "window.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

class Sample
{
public:
    Sample() = default;
    Sample(int argc, char** argv);
    virtual ~Sample() = default;

public:
    int exec();
    void* getNativeWindow();

protected:
    virtual void draw() = 0;

private:
    void mainLoop();

private:
    std::unique_ptr<Window> m_window = nullptr;

public:
    static std::filesystem::path getPath()
    {
        return path;
    }
    static std::filesystem::path getDir()
    {
        return dir;
    }

private:
    static std::filesystem::path path;
    static std::filesystem::path dir;
};
