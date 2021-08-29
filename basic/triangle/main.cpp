#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
const bool enableValidationLayers = true;
const bool enableDebugMessenger = true;
#else
const bool enableValidationLayers = false;
const bool enableDebugMessenger = false;
#endif


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData) {
    std::cerr << "messageSeverity : " << messageSeverity << std::endl;
    std::cerr << "messageType : " << messageType << std::endl;
    std::cerr << "pCallbackData->pMessage : " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class HelloTriangleApplication {


public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* m_pWindow;
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT m_debugMessengerUtilsCreateInfo;

    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_pickedPhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_logicalDevice;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

private:
    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(m_pWindow)) {
            glfwPollEvents();
        }
    }

    void cleanup() {

        vkDestroyDevice(m_logicalDevice, nullptr);

        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        vkDestroyInstance(m_instance, nullptr);

        glfwDestroyWindow(m_pWindow);

        glfwTerminate();

    }

    void createInstance() {
        const std::vector<const char*> requiredValidationLayers = getRequiredValidationLayers();
        if (enableValidationLayers && !checkValidationLayerSupport(requiredValidationLayers)) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        const std::vector<const char*> requiredExtensions = getRequiredExtensions();
        if(!checkExtensionSupport(requiredExtensions)) {
            throw std::runtime_error("extensions requested, but not available!");
        }

        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Triangle App";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Prototype";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.pNext = nullptr;
        if (enableDebugMessenger) {
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
            populateDefaultDebugUtilsMessengerCreateInfo(m_debugMessengerUtilsCreateInfo);
            instanceCreateInfo.pNext = (const void*)&m_debugMessengerUtilsCreateInfo;
        }

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void pickPhysicalDevice() {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);
        std::cout << "Physical Device Count : " << physicalDeviceCount << std::endl;
        if (physicalDeviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());

        for (const VkPhysicalDevice& physicalDevice : physicalDevices) {
            if (isDeviceSuitable(physicalDevice)) {
                m_pickedPhysicalDevice = physicalDevice;
                break;
            }
        }

        if (m_pickedPhysicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice physicalDevice) {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        return queueFamilyIndices.isComplete();
    }

    const std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableDebugMessenger) {
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        std::cout << "Required extensions :" << std::endl;
        for (const auto& extension : requiredExtensions) {
            std::cout << '\t' << extension << std::endl;
        }

        return requiredExtensions;
    }

    bool checkExtensionSupport(const std::vector<const char*> requiredExtensions) {

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

        // log available extensions;
        std::cout << "Available Extensions Count : " << availableExtensions.size() << std::endl;
        std::cout << "Available Extensions : " << std::endl;
        for(const auto& availableExtension : availableExtensions) {
            std::cout << '\t' << availableExtension.extensionName << std::endl;
        }

        for (const auto& requiredExtension : requiredExtensions) {
            bool extensionFound = false;
            for(const auto& availableExtension : availableExtensions) {
                if(strcmp(requiredExtension, availableExtension.extensionName) == 0) {
                    extensionFound = true;
                    break;
                }
            }

            if(!extensionFound) {
                return false;
            }
        }

        return true;
    }

    const std::vector<const char*> getRequiredValidationLayers() {
        std::vector<const char*> validationLayers;        
        validationLayers.push_back("VK_LAYER_KHRONOS_validation");

        std::cout << "Required Validataion Layers :" << std::endl;
        for (const auto& validationLayer : validationLayers) {
            std::cout << '\t' << validationLayer << std::endl;
        }

        return validationLayers;
    }

    bool checkValidationLayerSupport(const std::vector<const char*> validationLayers) {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        std::cout << "Available Validation Layer Count : \n\t" << availableLayers.size() << std::endl;
        std::cout << "Available Aalidation Layer : " << std::endl;
        for (const VkLayerProperties& layerProperties : availableLayers) {
            std::cout << "\t " << layerProperties.layerName << std::endl;
        }

        for (const char* layerName : validationLayers) {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDefaultDebugUtilsMessengerCreateInfo(createInfo);
        if (CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        if (!enableValidationLayers) return;
        
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void populateDefaultDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.flags = 0;
        createInfo.pNext = nullptr;
        createInfo.pUserData = nullptr; // Optional
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices queueFamilyIndices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        std::cout << queueFamilies.size() << std::endl;
        for(size_t i = 0; i < queueFamilies.size(); i++) {
            const auto& queueFamily = queueFamilies[i];
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueFamilyIndices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

            if (presentSupport) {
                queueFamilyIndices.presentFamily = i;
            }

            if (queueFamilyIndices.isComplete()) {
                break;
            }
        }

        return queueFamilyIndices;
    }

    void createLogicalDevice() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_pickedPhysicalDevice);
        std::set<uint32_t> uniqueQueueFamilieIndices = {queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value()};

        std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;

        float queuePriority = 1.0f;
        for(const uint32_t queueFamilyIndex: uniqueQueueFamilieIndices) {
            VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
            deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            deviceQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
            deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
        }

        VkPhysicalDeviceFeatures physicalDeviceFeatures{};

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

        deviceCreateInfo.enabledExtensionCount = 0;

        if (enableValidationLayers) {
            const std::vector<const char*> validationLayers = getRequiredValidationLayers();
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            deviceCreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_pickedPhysicalDevice, &deviceCreateInfo, nullptr, &m_logicalDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);
    }

    void createSurface() {
    if (glfwCreateWindowSurface(m_instance, m_pWindow, nullptr, &m_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}