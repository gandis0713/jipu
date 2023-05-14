#include "application.h"

#include "vkt/gpu/vulkan/vulkan_buffer.h"
#include "vkt/gpu/vulkan/vulkan_device.h"
#include "vkt/gpu/vulkan/vulkan_physical_device.h"
#include "vkt/gpu/vulkan/vulkan_pipeline.h"
#include "vkt/gpu/vulkan/vulkan_render_pass.h"
#include "vkt/gpu/vulkan/vulkan_swapchain.h"
#include "vkt/gpu/vulkan/vulkan_texture_view.h"

#include "utils/log.h"
#include "window.h"
#include <glm/glm.hpp>
#include <string>

std::filesystem::path vkt::Application::path;
std::filesystem::path vkt::Application::dir;

#ifdef NDEBUG
const bool enableValidationLayers = true;
const bool enableDebugMessenger = true;
#else
const bool enableValidationLayers = false;
const bool enableDebugMessenger = false;
#endif

const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData)
{
    if (messageType != 1)
    {
        LOG_ERROR("pCallbackData->pMessage : {}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

using namespace vkt;
namespace fs = std::filesystem;

Application::Application(int argc, char** argv)
{
    path = std::filesystem::path(argv[0]);
    dir = path.parent_path();
}

void Application::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void Application::initWindow()
{
    m_window = new Window(800, 600, "Triangle Window");
}

void Application::initVulkan()
{
    // create Driver.
    {
        DriverDescriptor descriptor{ .type = DRIVER_TYPE::VULKAN };
        m_driver = Driver::create(descriptor);
    }

    // create surface
    {
        SurfaceDescriptor descriptor{ .windowHandle = m_window->getNativeWindow() };
        m_surface = m_driver->createSurface(descriptor);
    }

    // create PhysicalDevice.
    {
        PhysicalDeviceDescriptor descriptor{};
        m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
    }

    // create Device.
    {
        DeviceDescriptor descriptor{};
        m_device = m_physicalDevice->createDevice(descriptor);
    }

    // create swapchain
    {
        SwapChainDescriptor swapChainCreateInfo{ .textureFormat = TextureFormat::kBGRA_8888_UInt_Norm,
                                                 .presentMode = PresentMode::kFifo,
                                                 .colorSpace = ColorSpace::kSRGBNonLinear,
                                                 .width = 800,
                                                 .height = 600,
                                                 .surface = m_surface.get() };
        m_swapChain = m_device->createSwapChain(swapChainCreateInfo);
    }

    // create buffer
    {
        m_vertices = {
            { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
            { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
        };

        uint64_t size = static_cast<uint64_t>(sizeof(Vertex) * m_vertices.size());
        BufferDescriptor bufferDescriptor{ .size = size,
                                           .usage = BufferUsage::kVertex };
        m_buffer = m_device->createBuffer(bufferDescriptor);

        void* mappedPointer = m_buffer->map();
        memcpy(mappedPointer, m_vertices.data(), size);
        m_buffer->unmap();
    }

    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSemaphores();
}

void Application::mainLoop()
{
    while (!m_window->shouldClose())
    {
        drawFrame();
    }

    // vkDeviceWaitIdle(m_context.device);
}

void Application::cleanup()
{
    // vkDestroySemaphore(m_context.device, m_renderFinishedSemaphore, nullptr);
    // vkDestroySemaphore(m_context.device, m_imageAvailableSemaphore, nullptr);

    // vkDestroyCommandPool(m_context.device, m_commandPool, nullptr);

    // for (auto framebuffer : m_vecSwapChainFramebuffers)
    // {
    //     vkDestroyFramebuffer(m_context.device, framebuffer, nullptr);
    // }

    //    m_pipeline.destroy();

    //    vkDestroyRenderPass(m_context.device, m_renderPass, nullptr);

    //    for (const VkImageView& imageView : m_swapChain->getImageViews()) // TODO: move into SwapChain object.
    //    {
    //        vkDestroyImageView(m_context.device, imageView, nullptr);
    //    }

    //    vkDestroySwapchainKHR(m_context.device, static_cast<VkSwapchainKHR>(m_swapChain->getHandle()), nullptr); //
    //    TODO: move into SwapChain object.

    //    vkDestroyDevice(m_context.device, nullptr);

    // DestroyDebugUtilsMessengerEXT(m_context.instance, m_debugMessenger, nullptr);

    //    vkDestroySurfaceKHR(m_context.instance, m_context.surface, nullptr);

    //    m_context.finalize(); // vkDestroyInstance(m_context.instance, nullptr);

    delete m_window; // glfwDestroyWindow(m_window);

    glfwTerminate();
}

void Application::createGraphicsPipeline()
{
    // create pipeline
    {
        PipelineDescriptor descriptor{};
        m_pipeline = m_device->createPipeline(descriptor);
    }

    auto vulkanPipeline = downcast(m_pipeline.get());
    auto vulkanDevice = downcast(m_device.get());
    auto vulkanRenderPass = vulkanDevice->getRenderPass(m_renderPassDescriptor);
    vulkanPipeline->setRenderPass(vulkanRenderPass);

    vulkanPipeline->createGraphicsPipeline((Application::getDir() / "triangle_vert.spv").generic_string(),
                                           (Application::getDir() / "triangle_frag.spv").generic_string());
}

void Application::createRenderPass()
{
    // create render pass descriptor.
    {
        m_renderPassDescriptor.format = VK_FORMAT_B8G8R8A8_SRGB;
        m_renderPassDescriptor.samples = VK_SAMPLE_COUNT_1_BIT;

        m_renderPassDescriptor.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        m_renderPassDescriptor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    }
}

void Application::createFramebuffers()
{
    VulkanDevice* vulkanDevice = downcast(m_device.get());
    VulkanRenderPass* renderPass = vulkanDevice->getRenderPass(m_renderPassDescriptor);
    auto vulkanSwapChain = downcast(m_swapChain.get());

    auto swapChainTextureViews = vulkanSwapChain->getTextureViews();

    for (size_t i = 0; i < swapChainTextureViews.size(); ++i)
    {
        VulkanTextureView* textureView = downcast(swapChainTextureViews[i]);

        VulkanFramebufferDescriptor descriptor{ renderPass->getVkRenderPass(),
                                                { textureView->getImageView() },
                                                vulkanSwapChain->getWidth(),
                                                vulkanSwapChain->getHeight() };

        m_framebufferDescriptors.push_back(descriptor);

        [[maybe_unused]] auto framebuffer = vulkanDevice->getFrameBuffer(descriptor); // pre-generated.
    }
}

void Application::createCommandPool()
{
    VulkanDevice* vulkanDevice = downcast(m_device.get());

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = vulkanDevice->getQueueIndex();
    commandPoolCreateInfo.flags = 0; // Optional

    if (vulkanDevice->vkAPI.CreateCommandPool(vulkanDevice->getVkDevice(), &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        LOG_ERROR("failed to create command pool!");
    }
}

void Application::createCommandBuffers()
{
    m_vecCommandBuffers.resize(m_framebufferDescriptors.size());

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_vecCommandBuffers.size());

    VulkanDevice* vulkanDevice = downcast(m_device.get());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;
    if (vkAPI.AllocateCommandBuffers(vulkanDevice->getVkDevice(), &commandBufferAllocateInfo, m_vecCommandBuffers.data()) != VK_SUCCESS)
    {
        LOG_ERROR("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < m_vecCommandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;                  // Optional
        commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkAPI.BeginCommandBuffer(m_vecCommandBuffers[i], &commandBufferBeginInfo) != VK_SUCCESS)
        {
            LOG_ERROR("failed to begin recording command buffer!");
        }

        auto vulkanDevice = downcast(m_device.get());
        auto vulkanRenderPass = vulkanDevice->getRenderPass(m_renderPassDescriptor);
        auto vulkanFrameBuffer = vulkanDevice->getFrameBuffer(m_framebufferDescriptors[i]);
        auto vulkanSwapChain = downcast(m_swapChain.get());

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vulkanRenderPass->getVkRenderPass();
        renderPassInfo.framebuffer = vulkanFrameBuffer->getVkFrameBuffer();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { vulkanSwapChain->getWidth(), vulkanSwapChain->getHeight() };

        VkClearValue clearValue = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;

        vkAPI.CmdBeginRenderPass(m_vecCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        auto vulkanPipeline = downcast(m_pipeline.get());
        vkAPI.CmdBindPipeline(m_vecCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getVkPipeline());

        auto vulkanBuffer = downcast(m_buffer.get());
        VkBuffer vertexBuffers[] = { vulkanBuffer->getVkBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkAPI.CmdBindVertexBuffers(m_vecCommandBuffers[i], 0, 1, vertexBuffers, offsets);

        vkAPI.CmdDraw(m_vecCommandBuffers[i], static_cast<uint32_t>(m_vertices.size()), 1, 0, 0);

        vkAPI.CmdEndRenderPass(m_vecCommandBuffers[i]);

        if (vkAPI.EndCommandBuffer(m_vecCommandBuffers[i]) != VK_SUCCESS)
        {
            LOG_ERROR("failed to record command buffer!");
        }
    }
}

void Application::drawFrame()
{
    uint32_t imageIndex;
    auto vulkanSwapChain = downcast(m_swapChain.get());
    VkSwapchainKHR swapChain = vulkanSwapChain->getVkSwapchainKHR();

    VulkanDevice* vulkanDevice = downcast(m_device.get());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;
    vkAPI.AcquireNextImageKHR(vulkanDevice->getVkDevice(), swapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
    VkPipelineStageFlags waitPipelineStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitPipelineStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_vecCommandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkQueue queue = vulkanDevice->getQueue();
    if (vkAPI.QueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        LOG_ERROR("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkAPI.QueuePresentKHR(queue, &presentInfo);

    vkAPI.QueueWaitIdle(vulkanDevice->getQueue());
}

void Application::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VulkanDevice* vulkanDevice = downcast(m_device.get());
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;
    if (vkAPI.CreateSemaphore(vulkanDevice->getVkDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
        vkAPI.CreateSemaphore(vulkanDevice->getVkDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS)
    {
        LOG_ERROR("failed to create semaphores!");
    }
}
