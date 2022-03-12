#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// #include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
namespace 
{
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    GLFWwindow* pWindow = nullptr;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSurfaceFormatKHR surfaceFormat {};
    VkSurfaceCapabilitiesKHR surfaceCapabilities {};
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlagBit;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages {};
    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers {};
    std::vector<VkFence> fences {};
    std::vector<VkSemaphore> semaphores {};

    constexpr int32_t WIDTH = 1024;
    constexpr int32_t HEIGHT = 512; 
    constexpr float graphicsQueuePriority = 1.0f;

    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;

    enum class FenceType: uint8_t
    {
        IMAGE_AVAILABLE = 0,
        RENDERING_DONE = 1,
        FENCE_TYPE_COUNT
    };

    enum class SemaphoreType: uint8_t
    {
        IMAGE_AVAILABLE = 0,
        RENDERING_DONE = 1,
        SEMAPHORE_TYPE_COUNT
    };


    std::vector<const char*> instanceExtensionNames
    {
        "VK_KHR_surface",
        "VK_KHR_xcb_surface" // for glfw on linux(ubuntu)
    };

    std::vector<const char*> deviceExtensionNames
    {
        "VK_KHR_swapchain"
    };
}

// create
VkResult checkInstanceLayer();
VkResult checkInstanceExtension();
VkResult createInstance();
VkResult createPhysicalDevice();
VkResult createDevice();
bool createWindow();
VkResult createSurface();
VkResult checkSurfaceSupport(const uint32_t queueFamilyIndex, VkBool32& supported);
VkResult checkSurfacePresentMode();
VkResult checkSurfaceFormat();
VkResult checkSurfaceCapability();
VkResult createSwapchain();
VkResult getSwapchainImages();
VkResult createCommandPool();
VkResult AllocateCommandBuffer();
VkResult setCommandBuffer();
VkResult createFence();
VkResult createSemaphore();

void render();

int main()
{
    VkResult result = VK_SUCCESS;

    // check instance extension.
    result = checkInstanceExtension();
    if(result != VK_SUCCESS)
    {
        return -1;
    }
    
    // check instance layer.
    result = checkInstanceLayer();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    // create instance.
    result = createInstance();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    // create physical device
    result = createPhysicalDevice();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    // create device
    result = createDevice();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);

    if(false == createWindow())
    {
        return -1;
    }

    result = createSurface();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    VkBool32 surfaceSupported;
    result = checkSurfaceSupport(graphicsQueueFamilyIndex, surfaceSupported);
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = checkSurfacePresentMode();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = checkSurfaceFormat();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = checkSurfaceCapability();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = createSwapchain();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = getSwapchainImages();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = createCommandPool();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = createFence();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = createSemaphore();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    result = AllocateCommandBuffer();
    if(result != VK_SUCCESS)
    {
        return -1;
    }

    // render just one time to test
    render();

    while (!glfwWindowShouldClose(pWindow)) {
        glfwPollEvents();
        // render();
    }


    // destroy
    for(VkSemaphore& semaphore: semaphores)
    {
        vkDestroySemaphore(device, semaphore, nullptr);
    }
    for(VkFence& fence: fences)
    {
        vkDestroyFence(device, fence, nullptr);
    }
    vkFreeCommandBuffers(device, commandPool, 1, commandBuffers.data());
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(pWindow);
    glfwTerminate();

    return 0;
}

VkResult checkInstanceLayer()
{
    uint32_t layerCount {0};
    VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if (result != VK_SUCCESS) 
    {
        std::cerr << "Failed to get layer count [Error code : " << result  << "]" << std::endl;
        return result;
    }

    std::vector<VkLayerProperties> layerProperties;
    layerProperties.resize(static_cast<std::size_t>(layerCount));

    result = vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
    if (result != VK_SUCCESS) 
    {
        std::cerr << "Failed to get layer properties [Error code : " << result  << "]" << std::endl;
        return result;
    }

    for (const VkLayerProperties& p : layerProperties) 
    {
        std::cout << "Layer Name             : " << p.layerName             << std::endl
                  << "Spec Version           : " << p.specVersion           << std::endl
                  << "Implementation Version : " << p.implementationVersion << std::endl
                  << "Description            : " << p.description           << std::endl;
        std::cout << std::endl;
    }

    return result;
}

VkResult checkInstanceExtension()
{
    VkResult result = VK_SUCCESS;

    uint32_t instanceExtensionCount {0};

    result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to get instance extension count [Error code : " << result << "]" << std::endl;
        return result;
    }

    std::vector<VkExtensionProperties> extensionProperties;
    extensionProperties.resize(instanceExtensionCount);

    result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, extensionProperties.data());
    for(uint32_t index = 0; index < extensionProperties.size(); ++index)
    {
        std::cout << "[Instance extension]" << std::endl
                  << "Name : " << extensionProperties[index].extensionName << std::endl
                  << "specVersion : " << extensionProperties[index].specVersion << std::endl;
    }

    return result;
}

VkResult createInstance()
{
    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    
    // extensions
    instanceCreateInfo.enabledExtensionCount = instanceExtensionNames.size();
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensionNames.data();

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);        
    if (result != VK_SUCCESS) 
    {
        std::cerr << "Failed to create instance [Error code : " << result  << "]" << std::endl;
        return result;
    }

    return result;
}

VkResult createPhysicalDevice()
{
    uint32_t physicalDeviceCount {0};
    VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to get physical device count [Error code : " << result  << "]" << std::endl;
        return result;
    }

    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(static_cast<std::size_t>(physicalDeviceCount));

    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to get physical devices [Error code : " << result  << "]" << std::endl;
        return result;
    }

    for(const VkPhysicalDevice& physicalDevice : physicalDevices)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;

        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        std::cout << physicalDeviceProperties.deviceName << std::endl;
    }

    for(const VkPhysicalDevice& physicalDevice : physicalDevices)
    {
        uint32_t deviceExtensionCount {0};

        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

        std::cout << "deviceExtensionCount : " << deviceExtensionCount << std::endl;

        std::vector<VkExtensionProperties> deviceExtensionProperties;
        deviceExtensionProperties.resize(deviceExtensionCount);

        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensionProperties.data());

        for(uint32_t index = 0; index < deviceExtensionProperties.size(); ++index)
        {
            std::cout << "[Device Extension]" << std::endl
                      << "Name    : " << deviceExtensionProperties[index].extensionName << std::endl
                      << "Version : " << deviceExtensionProperties[index].specVersion << std::endl;
        }
    }

    physical_device = physicalDevices[0];

    return result;
}

VkResult createDevice()
{
    VkResult result = VK_SUCCESS;

    uint32_t queueFamilyPropertyCount {0};
    
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyPropertyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    queueFamilyProperties.resize(queueFamilyPropertyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyPropertyCount, queueFamilyProperties.data());

    bool graphicsQueueFlag = false;
    bool computeQueueFlag = false;
    bool transferQueueFlag = false;
    bool sparseQueueFlag = false;
    for(uint32_t index = 0; index < queueFamilyProperties.size(); ++index)
    {
        std::cout << std::endl;

        graphicsQueueFlag = (queueFamilyProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT);
        computeQueueFlag = (queueFamilyProperties[index].queueFlags & VK_QUEUE_COMPUTE_BIT);
        transferQueueFlag = (queueFamilyProperties[index].queueFlags & VK_QUEUE_TRANSFER_BIT);
        sparseQueueFlag = (queueFamilyProperties[index].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT);

        std::cout << std::boolalpha << "graphicsQueueFlag : " << graphicsQueueFlag << std::endl;
        std::cout << std::boolalpha << "computeQueueFlag : " << computeQueueFlag << std::endl;
        std::cout << std::boolalpha << "transferQueueFlag : " << transferQueueFlag << std::endl;
        std::cout << std::boolalpha << "sparseQueueFlag : " << sparseQueueFlag << std::endl;
        
        std::cout << "queueCount : " << queueFamilyProperties[index].queueCount << std::endl;

        if(queueFamilyProperties[index].queueCount < 1)
        {
            continue;
        }     

        if(graphicsQueueFlag)
        {
            graphicsQueueFamilyIndex = index;
            break;
        }
    }

    if(graphicsQueueFamilyIndex == 0xffff)
    {
        std::cerr << "There is no queue family for graphics" << std::endl;
        return VK_ERROR_UNKNOWN;
    }

    VkDeviceQueueCreateInfo deviceQueueCreateInfo {};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;

    VkDeviceCreateInfo deviceCreateInfo {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = deviceExtensionNames.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensionNames.data();

    result = vkCreateDevice(physical_device, &deviceCreateInfo, nullptr, &device);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create device [Error coce : " << result << "]" << std::endl;
    }

    return result;
}

bool createWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    if(nullptr == pWindow)
    {
        std::cerr << "Failed to create window" << std::endl;
    }
    
    return pWindow != nullptr;
}

VkResult createSurface() 
{
    VkResult result = glfwCreateWindowSurface(instance, pWindow, nullptr, &surface);
    if (result != VK_SUCCESS) 
    {
        std::cerr << "Failed to create surface [Error code : " << result << "]" << std::endl;
    }

    return result;
}

VkResult checkSurfaceSupport(const uint32_t queueFamilyIndex, VkBool32 &supported)
{
    supported = false;

    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, graphicsQueueFamilyIndex, surface, &supported);
    if(VK_SUCCESS != result)
    {
        std::cerr << "Failed to check surface supported [Error code : " << result << "]" << std::endl;
    }

    if(false == supported)
    {
        std::cerr << "Surface is not supported [QueueFamilyIndex : " << queueFamilyIndex << "]" << std::endl;
    }

    return result;
}

VkResult checkSurfacePresentMode()
{
    uint32_t surfacePresentModeCount;
    VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &surfacePresentModeCount, nullptr);

    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to check surface present mode [Error code : " << result << "]" << std::endl;
    }

    std::vector<VkPresentModeKHR> presentModes;
    presentModes.resize(surfacePresentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &surfacePresentModeCount, presentModes.data());
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to get surface present mode [Error code : " << result << "]" << std::endl;
    }

    std::cout << "[Present Mode]" << std::endl;
    for(const VkPresentModeKHR& mode : presentModes) 
    {
        switch(mode) 
        {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
            std::cout << "VK_PRESENT_MODE_IMMEDIATE_KHR" << std::endl;
            break;

            case VK_PRESENT_MODE_MAILBOX_KHR:
            std::cout << "VK_PRESENT_MODE_MAILBOX_KHR" << std::endl;
            break;

            case VK_PRESENT_MODE_FIFO_KHR:
            std::cout << "VK_PRESENT_MODE_FIFO_KHR" << std::endl;
            break;

            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            std::cout << "VK_PRESENT_MODE_FIFO_RELAXED_KHR" << std::endl;
            break;
        }
    }

    return result;
}

VkResult checkSurfaceFormat()
{
    uint32_t surfaceFormatCount {0};
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surfaceFormatCount, nullptr);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to get surface format count [Error code : " << result << "]" << std::endl;
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    surfaceFormats.resize(surfaceFormatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surfaceFormatCount, surfaceFormats.data());
    if(result != VK_SUCCESS)
    {
        std::cerr << "Faield to get surface format [Error code : " << result << "]" << std::endl;
    }

    std::cout << "[Surface Format]" << std::endl;
    for(uint32_t index = 0; index < surfaceFormats.size(); ++index)
    {
        std::cout << "Format      : " << surfaceFormats[index].format << std::endl;
        std::cout << "Color Space : " << surfaceFormats[index].colorSpace << std::endl;
    }

    // use first supoorted format
    if(surfaceFormatCount < 1)
    {
        std::cerr << "There is no surface format that supported." << std::endl;
        return VK_ERROR_UNKNOWN;
    }

    surfaceFormat = surfaceFormats[0];

    return result;
}

VkResult checkSurfaceCapability()
{
    uint32_t count;
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surfaceCapabilities);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to get surface capabilities [Error code : " << result << "]" << std::endl;
    }

    std::cout << "[Surface Capabilities]" << std::endl;
    std::cout << "maxImageCount : " << surfaceCapabilities.maxImageCount << std::endl;
    std::cout << "minImageCount : " << surfaceCapabilities.minImageCount << std::endl;
    std::cout << "currentExtent.width : " << surfaceCapabilities.currentExtent.width << std::endl;
    std::cout << "currentExtent.height : " << surfaceCapabilities.currentExtent.height << std::endl;
    std::cout << "supportedCompositeAlpha : " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
    std::cout << "supportedUsageFlags : " << surfaceCapabilities.supportedUsageFlags << std::endl;


    // check composite alpha flag.
    for(uint32_t index = 0; index < 32; ++index)
    {
        VkCompositeAlphaFlagBitsKHR currentCompositeAlphaFlagBit = static_cast<VkCompositeAlphaFlagBitsKHR>(0x1 << index);
        if(surfaceCapabilities.supportedUsageFlags & currentCompositeAlphaFlagBit)
        {
            compositeAlphaFlagBit = currentCompositeAlphaFlagBit;
            break;
        }
    }

    return result;
}

VkResult createSwapchain()
{
    VkSwapchainCreateInfoKHR swapchainCreateInfo {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = 2; // use double buffering.
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;

    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = compositeAlphaFlagBit;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkResult result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create swapchain." << std::endl;
    }

    return result;
}

VkResult getSwapchainImages()
{
    uint32_t swapchainImageCount {0};
    VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to get swapchaing image count. [Error Code : " << result << "]" << std::endl;
        return result;
    }

    swapchainImages.clear();
    swapchainImages.resize(swapchainImageCount);
    result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to get swapchain images. [Error Code : " << result << "]" << std::endl;
        return result;
    }

    std::cout << "Available swapchain image count : " << swapchainImages.size() << std::endl;

    return result;
}

VkResult createCommandPool()
{
    VkCommandPoolCreateInfo commandPoolCreateInfo {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

    VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create command pool [Error code : " << result << "]" << std::endl;
    }

    return result;
}

VkResult AllocateCommandBuffer()
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1; // use only one in current.

    commandBuffers.resize(1);
    VkCommandBuffer& commandBuffer = commandBuffers[0];
    VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to allocate command buffers [Error code : " << result << "]" << std::endl;
    }

    return result;
}

VkResult setCommandBuffer()
{
    VkCommandBuffer commandBuffer = commandBuffers[0];

    VkResult result = vkGetFenceStatus(device, fences[static_cast<uint8_t>(FenceType::RENDERING_DONE)]);
    if(result == VK_NOT_READY)
    {
        result = vkWaitForFences(device, 1, &fences[static_cast<uint8_t>(FenceType::RENDERING_DONE)], VK_TRUE, UINT64_MAX);
        if(result != VK_SUCCESS)
        {
            std::cerr << "Failed to wait fence for checking rendering done [Error code : " << result << "]" << std::endl;
            return result;
        }
    }

    result = vkResetFences(device, 1, &fences[static_cast<uint8_t>(FenceType::RENDERING_DONE)]);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to reset fence for rendering done. [Error code : " << result << std::endl;
        return result;
    }

    result = vkResetCommandBuffer(commandBuffer, 0);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to reset command buffer [Error code : " << result << "]" << std::endl;
        return result;
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to begin command buffer [Error code : " << result << "]" << std::endl;
    }

    return result;
}

VkResult createFence()
{
    // create fence to syncronize.
    VkFenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    fences.resize(static_cast<uint8_t>(FenceType::FENCE_TYPE_COUNT));

    // create image available fence.
    VkResult result = vkCreateFence(device, &fenceCreateInfo, nullptr, &fences[static_cast<uint8_t>(FenceType::IMAGE_AVAILABLE)]);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create image available fence [Error code : " << result << "]" << std::endl;
        return result;
    }

    result = vkCreateFence(device, &fenceCreateInfo, nullptr, &fences[static_cast<uint8_t>(FenceType::RENDERING_DONE)]);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create rendering done fence [Error code : " << result << "]" << std::endl;
        return result;
    }

    return result;
}

VkResult createSemaphore()
{
    VkSemaphoreCreateInfo semaphoreCreateInfo {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    semaphores.resize(static_cast<uint8_t>(SemaphoreType::SEMAPHORE_TYPE_COUNT));
    VkResult result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores[static_cast<uint8_t>(SemaphoreType::IMAGE_AVAILABLE)]);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create semaphore for checking image available [Error code : " << result << "]" << std::endl;
        return result;
    }

    result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores[static_cast<uint8_t>(SemaphoreType::RENDERING_DONE)]);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to create semaphore for checking rendering done [Error code : " << result << "]" << std::endl;
        return result;
    }

    return result;
}

void render()
{
    uint32_t swapchainImageIndex {0};
    VkResult result = vkAcquireNextImageKHR(device,
                                            swapchain,
                                            UINT64_MAX, 
                                            semaphores[static_cast<uint8_t>(SemaphoreType::IMAGE_AVAILABLE)], 
                                            VK_NULL_HANDLE, 
                                            &swapchainImageIndex);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to acquire swapchain image index [Error code : " << result << "]" << std::endl;
        return;
    }

    std::cout << "Acquired swapchain image index : " << swapchainImageIndex << std::endl;

    VkImage swapchainImage = swapchainImages[swapchainImageIndex];

    // set command buffer.
    result = setCommandBuffer();
    if(result != VK_SUCCESS)
    {
        return;
    }

    VkCommandBuffer commandBuffer = commandBuffers[0];

    // set image memory barrier.
    {
        VkImageMemoryBarrier imageMemoryBarrier {};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.srcQueueFamilyIndex = graphicsQueueFamilyIndex;
        imageMemoryBarrier.dstQueueFamilyIndex = graphicsQueueFamilyIndex;
        imageMemoryBarrier.image = swapchainImage;
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageMemoryBarrier.subresourceRange.levelCount = 1;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0,
                             0, nullptr,
                             0, nullptr,
                             1, &imageMemoryBarrier);
    }

    // set clear color value command
    VkClearColorValue clearColorValue {};
    clearColorValue.float32[0] = 1.0f;
    clearColorValue.float32[1] = 0.0f;
    clearColorValue.float32[2] = 0.0f;
    clearColorValue.float32[3] = 1.0f;

    VkImageSubresourceRange imageSubresourceRange {};
    imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageSubresourceRange.levelCount = 1;
    imageSubresourceRange.layerCount = 1;
    
    vkCmdClearColorImage(commandBuffer,
                         swapchainImage,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         &clearColorValue,
                         1,
                         &imageSubresourceRange);

    // set image memory barrier.
    {
        VkImageMemoryBarrier imageMemoryBarrier;
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        imageMemoryBarrier.dstAccessMask = 0;
        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        imageMemoryBarrier.srcQueueFamilyIndex = graphicsQueueFamilyIndex;
        imageMemoryBarrier.dstQueueFamilyIndex = graphicsQueueFamilyIndex;
        imageMemoryBarrier.image = swapchainImage;
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageMemoryBarrier.subresourceRange.levelCount = 1;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(commandBuffer,
                            VK_PIPELINE_STAGE_TRANSFER_BIT,
                            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                            0,
                            0, nullptr,
                            0, nullptr,
                            1, &imageMemoryBarrier);
    }

    // end command buffer recording.
    vkEndCommandBuffer(commandBuffer);

    constexpr VkPipelineStageFlags waitDstStateMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pWaitSemaphores = &semaphores[static_cast<uint8_t>(SemaphoreType::IMAGE_AVAILABLE)];
    submitInfo.pWaitDstStageMask = &waitDstStateMask;
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &semaphores[static_cast<uint8_t>(SemaphoreType::RENDERING_DONE)];

    // submit command buffer to queue.
    result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, fences[static_cast<uint8_t>(FenceType::RENDERING_DONE)]);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to submit to queue [Error code " << result << "]" << std::endl;
    }

    // wait device idle state after executed command.
    result = vkDeviceWaitIdle(device);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to wait device idle state. [Error code : " << result << "]" << std::endl;
        return;
    }

    // render output 
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &swapchainImageIndex;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &semaphores[static_cast<uint8_t>(SemaphoreType::RENDERING_DONE)];

    result = vkQueuePresentKHR(graphicsQueue, &presentInfo);
    if(result != VK_SUCCESS)
    {
        std::cerr << "Failed to pass present information to queue [Error code : " << result << result << std::endl;
        return;
    }

    std::cout << static_cast<uint8_t>(FenceType::FENCE_TYPE_COUNT) << std::endl;
}

