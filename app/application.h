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
#include "vkt/gpu/vulkan/vulkan_framebuffer.h"
#include "vkt/gpu/vulkan/vulkan_render_pass.h"

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

using namespace vkt;

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
};

class Application
{
public:
    Application() = default;
    Application(int argc, char** argv);

public:
    void run();

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

private:
    void initWindow();
    void initVulkan();

    void mainLoop();
    void cleanup();

    void createRenderPipeline();
    void createCommandBuffers();
    void createSemaphores();

    void drawFrame();

private:
    std::unique_ptr<Window> m_window = nullptr;

    // data
    std::vector<Vertex> m_vertices{};

    // wrapper
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;

    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;

    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<RenderPipeline> m_renderPipeline = nullptr;

    std::unique_ptr<ShaderModule> m_vertexShaderModule = nullptr;
    std::unique_ptr<ShaderModule> m_fragmentShaderModule = nullptr;

    std::unique_ptr<Queue> m_renderQueue = nullptr;

    std::unique_ptr<Buffer> m_buffer = nullptr;

    std::vector<std::unique_ptr<CommandBuffer>> m_commandBuffers{};

    // sync
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
};
