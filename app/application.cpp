#include "application.h"
#include "window.h"
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

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    if (messageType != 1)
    {
        LOG_ERROR("pCallbackData->pMessage : {}", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

using namespace vkt;
namespace fs = std::filesystem;

void Application::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void Application::initWindow() { m_window = new Window(800, 600, "Triangle Window"); }

void Application::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    createSurface();
    createSwapChain();
    createImageViews();
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

    vkDeviceWaitIdle(m_context.device);
}

void Application::cleanup()
{
    vkDestroySemaphore(m_context.device, m_renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(m_context.device, m_imageAvailableSemaphore, nullptr);

    vkDestroyCommandPool(m_context.device, m_commandPool, nullptr);

    for (auto framebuffer : m_vecSwapChainFramebuffers)
    {
        vkDestroyFramebuffer(m_context.device, framebuffer, nullptr);
    }

    m_pipeline.destroy();

    vkDestroyRenderPass(m_context.device, m_renderPass, nullptr);

    for (const VkImageView& imageView : m_vecSwapChainImageViews)
    {
        vkDestroyImageView(m_context.device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(m_context.device, m_swapChain, nullptr);

    vkDestroyDevice(m_context.device, nullptr);

    DestroyDebugUtilsMessengerEXT(m_context.instance, m_debugMessenger, nullptr);

    vkDestroySurfaceKHR(m_context.instance, m_context.surface, nullptr);

    m_context.finalize(); // vkDestroyInstance(m_context.instance, nullptr);

    delete m_window; // glfwDestroyWindow(m_window);

    glfwTerminate();
}

void Application::createInstance() { m_context.initialize(); }

const std::vector<const char*>& Application::getRequiredValidationLayers()
{
    static std::vector<const char*> requiredValidationLayers;

    if (requiredValidationLayers.size() == 0)
    {
        if (enableValidationLayers)
        {
            // requiredValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        LOG_DEBUG("Required Validataion Layers :");
        for (const auto& validationLayer : requiredValidationLayers)
        {
            LOG_DEBUG("  : {}", validationLayer);
        }
    }

    return requiredValidationLayers;
}

bool Application::checkValidationLayerSupport(const std::vector<const char*> validationLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    LOG_DEBUG("Available Validation Layer Count : {}", availableLayers.size());
    LOG_DEBUG("Available Aalidation Layer : ");
    for (const VkLayerProperties& layerProperties : availableLayers)
    {
        LOG_DEBUG("  : {}", layerProperties.layerName);
    }

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

void Application::setupDebugMessenger()
{
    if (!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDefaultDebugUtilsMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(m_context.instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult Application::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pDebugUtilsMessengerCreateInfoEXT,
                                                   const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugUtilsMessengerEXT)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pDebugUtilsMessengerCreateInfoEXT, pAllocator, pDebugUtilsMessengerEXT);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    if (!enableValidationLayers)
        return;

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void Application::populateDefaultDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo)
{
    debugUtilsMessengerCreateInfo = {};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback = debugCallback;
    debugUtilsMessengerCreateInfo.flags = 0;
    debugUtilsMessengerCreateInfo.pNext = nullptr;
    debugUtilsMessengerCreateInfo.pUserData = nullptr; // Optional
}

void Application::createSurface()
{
    VkSurfaceKHR surface = static_cast<VkSurfaceKHR>(m_window->createSurface(m_context.instance));
    if (surface == nullptr)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    m_context.surface = surface;
}

VkSurfaceFormatKHR Application::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
{
    for (const VkSurfaceFormatKHR& availableSurfaceFormat : availableSurfaceFormats)
    {
        if (availableSurfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableSurfaceFormat;
        }
    }

    return availableSurfaceFormats[0];
}

VkPresentModeKHR Application::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Application::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX)
    {
        return surfaceCapabilities.currentExtent;
    }
    else
    {
        int frameBufferWidth, frameBufferHeight;
        m_window->getFrameBufferSize(&frameBufferWidth, &frameBufferHeight);

        VkExtent2D actualImageExtent = { static_cast<uint32_t>(frameBufferWidth), static_cast<uint32_t>(frameBufferHeight) };

        actualImageExtent.width = std::clamp(actualImageExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        actualImageExtent.height = std::clamp(actualImageExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

        return actualImageExtent;
    }
}

void Application::createSwapChain()
{
    if (m_context.surface == VK_NULL_HANDLE)
    {
        std::runtime_error("Failed to create swap chain due to surface is null.");
    }

    SurfaceCreateInfo info{ m_context.physicalDevice, m_context.surface };
    m_surface = std::make_shared<Surface>(info);

    // TODO: use SwapChain object.

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(m_surface->getSurfaceFormats());
    VkPresentModeKHR presentMode = chooseSwapPresentMode(m_surface->getPresentModes());

    const VkSurfaceCapabilitiesKHR& surfaceCapabilities = m_surface->getSurfaceCapabilities();
    VkExtent2D imageExtent = chooseSwapExtent(surfaceCapabilities);

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = m_context.surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = imageExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices foundQueueFamilyIndices = QueueFamilyIndices::findQueueFamilies(m_context.physicalDevice);
    uint32_t queueFamilyIndices[] = { foundQueueFamilyIndices.graphicsFamily.value(), foundQueueFamilyIndices.presentFamily.value() };

    if (foundQueueFamilyIndices.graphicsFamily != foundQueueFamilyIndices.presentFamily)
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;     // Optional
        swapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;

    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_context.device, &swapchainCreateInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_context.device, m_swapChain, &imageCount, nullptr);
    m_vecSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_context.device, m_swapChain, &imageCount, m_vecSwapChainImages.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = imageExtent;
}

void Application::createImageViews()
{
    m_vecSwapChainImageViews.resize(m_vecSwapChainImages.size());

    for (size_t i = 0; i < m_vecSwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = m_vecSwapChainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_swapChainImageFormat;

        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_context.device, &imageViewCreateInfo, nullptr, &m_vecSwapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void Application::createGraphicsPipeline()
{
    m_pipeline.setDevice(m_context.device);
    m_pipeline.setRenderPass(m_renderPass);

    m_pipeline.createGraphicsPipeline((Application::getDir() / "triangle_vert.spv").generic_string(),
                                      (Application::getDir() / "triangle_frag.spv").generic_string());
}

void Application::createRenderPass()
{
    VkAttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.format = m_swapChainImageFormat;
    colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    if (vkCreateRenderPass(m_context.device, &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void Application::createFramebuffers()
{
    m_vecSwapChainFramebuffers.resize(m_vecSwapChainImageViews.size());

    for (size_t i = 0; i < m_vecSwapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = { m_vecSwapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = m_renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = m_swapChainExtent.width;
        framebufferCreateInfo.height = m_swapChainExtent.height;
        framebufferCreateInfo.layers = 1;

        if (vkCreateFramebuffer(m_context.device, &framebufferCreateInfo, nullptr, &m_vecSwapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void Application::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(m_context.physicalDevice);

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    commandPoolCreateInfo.flags = 0; // Optional

    if (vkCreateCommandPool(m_context.device, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

void Application::createCommandBuffers()
{
    m_vecCommandBuffers.resize(m_vecSwapChainFramebuffers.size());

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_vecCommandBuffers.size());

    if (vkAllocateCommandBuffers(m_context.device, &commandBufferAllocateInfo, m_vecCommandBuffers.data()) != VK_SUCCESS)
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
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_vecSwapChainFramebuffers[i];

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_swapChainExtent;

        VkClearValue clearValue = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(m_vecCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // vkCmdBindPipeline(m_vecCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
        m_pipeline.bindPipeline(m_vecCommandBuffers[i]);

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
    vkAcquireNextImageKHR(m_context.device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

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

    if (vkQueueSubmit(m_context.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(m_context.presentQueue, &presentInfo);

    vkQueueWaitIdle(m_context.presentQueue);
}

void Application::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(m_context.device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(m_context.device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create semaphores!");
    }
}
