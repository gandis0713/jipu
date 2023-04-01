#include "application.h"
#include "window.h"

#include "gpu/platform.h"
#include "gpu/vulkan/vulkan_adapter.h"
#include "gpu/vulkan/vulkan_device.h"
#include "gpu/vulkan/vulkan_pipeline.h"
#include "gpu/vulkan/vulkan_render_pass.h"
#include "gpu/vulkan/vulkan_swapchain.h"
#include "gpu/vulkan/vulkan_texture_view.h"
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
        DriverDescriptor descriptor{ DRIVER_TYPE::VULKAN };
        m_driver = Driver::create(descriptor);
    }

    // create Adapter.
    {
        AdapterDescriptor descriptor{};
        m_adapter = m_driver->createAdapter(descriptor);
    }

    // create Device.
    {
        DeviceDescriptor descriptor{};
        m_device = m_adapter->createDevice(descriptor);
    }

    // create platform
    {
        PlatformDescriptor descriptor{ m_window->getNativeWindow() };
        m_platform = m_adapter->createPlatform(descriptor);
    }

    // setupDebugMessenger();

    createSurface();
    createSwapChain();
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

// TODO: remove
// const std::vector<const char*>& Application::getRequiredValidationLayers()
// {
//     static std::vector<const char*> requiredValidationLayers;

//     if (requiredValidationLayers.size() == 0)
//     {
//         if (enableValidationLayers)
//         {
//             // requiredValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
//         }

//         LOG_DEBUG("Required Validataion Layers :");
//         for (const auto& validationLayer : requiredValidationLayers)
//         {
//             LOG_DEBUG("  : {}", validationLayer);
//         }
//     }

//     return requiredValidationLayers;
// }

// bool Application::checkValidationLayerSupport(const std::vector<const char*> validationLayers)
// {
//     uint32_t layerCount;
//     vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

//     std::vector<VkLayerProperties> availableLayers(layerCount);
//     vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

//     LOG_DEBUG("Available Validation Layer Count : {}", availableLayers.size());
//     LOG_DEBUG("Available Aalidation Layer : ");
//     for (const VkLayerProperties& layerProperties : availableLayers)
//     {
//         LOG_DEBUG("  : {}", layerProperties.layerName);
//     }

//     for (const char* layerName : validationLayers)
//     {
//         bool layerFound = false;
//         for (const auto& layerProperties : availableLayers)
//         {
//             if (strcmp(layerName, layerProperties.layerName) == 0)
//             {
//                 layerFound = true;
//                 break;
//             }
//         }

//         if (!layerFound)
//         {
//             return false;
//         }
//     }

//     return true;
// }

// void Application::setupDebugMessenger()
// {
//     if (!enableValidationLayers)
//         return;

//     VkDebugUtilsMessengerCreateInfoEXT createInfo;
//     populateDefaultDebugUtilsMessengerCreateInfo(createInfo);
//     if (CreateDebugUtilsMessengerEXT(m_context.instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
//     {
//         throw std::runtime_error("failed to set up debug messenger!");
//     }
// }

// VkResult Application::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT*
// pDebugUtilsMessengerCreateInfoEXT,
//                                                    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT*
//                                                    pDebugUtilsMessengerEXT)
// {
//     auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
//     "vkCreateDebugUtilsMessengerEXT"); if (func != nullptr)
//     {
//         return func(instance, pDebugUtilsMessengerCreateInfoEXT, pAllocator, pDebugUtilsMessengerEXT);
//     }
//     else
//     {
//         return VK_ERROR_EXTENSION_NOT_PRESENT;
//     }
// }

// void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const
// VkAllocationCallbacks* pAllocator)
// {
//     if (!enableValidationLayers)
//         return;

//     auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,
//     "vkDestroyDebugUtilsMessengerEXT"); if (func != nullptr)
//     {
//         func(instance, debugMessenger, pAllocator);
//     }
// }

// void Application::populateDefaultDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&
// debugUtilsMessengerCreateInfo)
// {
//     debugUtilsMessengerCreateInfo = {};
//     debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//     debugUtilsMessengerCreateInfo.messageSeverity =
//         VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
//         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
//     debugUtilsMessengerCreateInfo.messageType =
//         VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
//         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//     debugUtilsMessengerCreateInfo.pfnUserCallback = debugCallback;
//     debugUtilsMessengerCreateInfo.flags = 0;
//     debugUtilsMessengerCreateInfo.pNext = nullptr;
//     debugUtilsMessengerCreateInfo.pUserData = nullptr; // Optional
// }

void Application::createSurface()
{
    // create surface
    SurfaceDescriptor descriptor{};
    m_surface = m_platform->createSurface(descriptor);
}

void Application::createSwapChain()
{
    // create swapchain
    SwapChainDescriptor swapChainCreateInfo{ m_surface.get() };
    m_swapChain = m_device->createSwapChain(swapChainCreateInfo);
}

void Application::createGraphicsPipeline()
{
    // create pipeline
    {
        PipelineDescriptor descriptor{};
        m_pipeline = m_device->createPipeline(descriptor);
    }

    auto vulkanPipeline = static_cast<VulkanPipeline*>(m_pipeline.get());
    auto vulkanDevice = static_cast<VulkanDevice*>(m_device.get());
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
    VulkanDevice* vulkanDevice = static_cast<VulkanDevice*>(m_device.get());
    auto renderPass = vulkanDevice->getRenderPass(m_renderPassDescriptor);
    auto vulkanSwapChain = static_cast<VulkanSwapChain*>(m_swapChain.get());

    auto swapChainTextureViews = vulkanSwapChain->getTextureViews();

    for (size_t i = 0; i < swapChainTextureViews.size(); ++i)
    {
        VulkanTextureView* textureView = static_cast<VulkanTextureView*>(swapChainTextureViews[i]);

        VulkanFramebufferDescriptor descriptor{ renderPass->getRenderPass(),
                                                { textureView->getImageView() },
                                                vulkanSwapChain->getExtent2D().width,
                                                vulkanSwapChain->getExtent2D().height };

        m_framebufferDescriptors.push_back(descriptor);

        [[maybe_unused]] auto framebuffer = vulkanDevice->getFrameBuffer(descriptor); // pre-generated.
    }
}

void Application::createCommandPool()
{
    VkPhysicalDevice physicalDevice = static_cast<VulkanAdapter*>(m_adapter.get())->getPhysicalDevice();
    QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    commandPoolCreateInfo.flags = 0; // Optional

    VkDevice device = static_cast<VulkanDevice*>(m_device.get())->getDevice();
    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
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

    VkDevice device = static_cast<VulkanDevice*>(m_device.get())->getDevice();
    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, m_vecCommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < m_vecCommandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;                  // Optional
        commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(m_vecCommandBuffers[i], &commandBufferBeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

        auto vulkanDevice = static_cast<VulkanDevice*>(m_device.get());
        auto vulkanRenderPass = vulkanDevice->getRenderPass(m_renderPassDescriptor);
        renderPassInfo.renderPass = vulkanRenderPass->getRenderPass();

        VulkanFrameBuffer* vulkanFrameBuffer = vulkanDevice->getFrameBuffer(m_framebufferDescriptors[i]);
        renderPassInfo.framebuffer = vulkanFrameBuffer->getVkFrameBuffer();

        renderPassInfo.renderArea.offset = { 0, 0 };
        auto vulkanSwapChain = static_cast<VulkanSwapChain*>(m_swapChain.get());
        renderPassInfo.renderArea.extent = vulkanSwapChain->getExtent2D();

        VkClearValue clearValue = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(m_vecCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // vkCmdBindPipeline(m_vecCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
        auto vulkanPipeline = static_cast<VulkanPipeline*>(m_pipeline.get());
        vulkanPipeline->bindPipeline(m_vecCommandBuffers[i]);

        vkCmdDraw(m_vecCommandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(m_vecCommandBuffers[i]);

        if (vkEndCommandBuffer(m_vecCommandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void Application::drawFrame()
{
    uint32_t imageIndex;
    auto vulkanSwapChain = static_cast<VulkanSwapChain*>(m_swapChain.get());
    VkSwapchainKHR swapChain = vulkanSwapChain->getVkSwapchainKHR();

    VulkanDevice* vulkanDevice = static_cast<VulkanDevice*>(m_device.get());
    VkDevice device = vulkanDevice->getDevice();
    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

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

    if (vkQueueSubmit(vulkanDevice->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
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

    vkQueuePresentKHR(vulkanDevice->getPresentQueue(), &presentInfo);

    vkQueueWaitIdle(vulkanDevice->getPresentQueue());
}

void Application::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkDevice device = static_cast<VulkanDevice*>(m_device.get())->getDevice();
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create semaphores!");
    }
}
