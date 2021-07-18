#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance instance;

private:
    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
        const std::vector<const char*> validationLayers = getRequiredValidationLayers();
        if (enableValidationLayers && !checkValidationLayerSupport(validationLayers)) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        const std::vector<const char*> requiredExtensions = getRequiredExtensions();
        if(!checkExtensionSupport(requiredExtensions)) {
            throw std::runtime_error("extensions requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Triangle App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Prototype";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        createInfo.enabledLayerCount = 0;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    const std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
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

        std::cout << "Available Validation Layer Count :" << availableLayers.size() << std::endl;
        std::cout << "Available Aalidation Layer : " << std::endl;
        for (const auto& layerProperties : availableLayers) {
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