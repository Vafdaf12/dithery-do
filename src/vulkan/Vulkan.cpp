#include "Vulkan.h"
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>

#include "vulkan_ext.h"

#ifdef NDEBUG
constexpr bool ENABLE_VALIDATION = false;
#else
constexpr bool ENABLE_VALIDATION = true;
#endif

#define EXPECT(cond, message) if(!(cond)) throw std::runtime_error(message);

struct QueueFamilyIndices {
    std::optional<uint32_t> computeFamily;
    bool isComplete() const {
        return computeFamily.has_value();
    }
};
QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) {
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies;
    queueFamilies = device.getQueueFamilyProperties();
    int i = 0;
    for(const auto& family : queueFamilies) {
        if(family.queueFlags & vk::QueueFlagBits::eCompute) {
            indices.computeFamily = i;
        }
        i++;
    }

    return indices;
}

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char*> EXTENSIONS = {
#ifndef NDEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
};

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
) {
    if(messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) return VK_FALSE;
    std::cout << "[VALIDATION] ";
    std::cout << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}
VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = debugCallback;
    return createInfo;
}

bool Vulkan::supportsLayers(const std::vector<const char*>& layers) const {
    std::vector<vk::LayerProperties> availableLayers;
    availableLayers = vk::enumerateInstanceLayerProperties();

    // check if all layers are supported
    // TODO: refactor to use C++ STL algorithms
    for(const char* name: layers) {
        bool found = false;
        for(const auto& props : availableLayers) {
            if(strcmp(name, props.layerName) == 0) {
                found = true;
                break;
            }
        }
        if(!found) return false;
    }
    return true;
}

bool Vulkan::supportsExtensions(const std::vector<const char*>& layers) const {
    std::vector<vk::ExtensionProperties> availableExtensions;
    availableExtensions = vk::enumerateInstanceExtensionProperties();

    // check if all layers are supported
    // TODO: refactor to use C++ STL algorithms
    for(const char* name: layers) {
        bool found = false;
        for(const auto& props : availableExtensions) {
            if(strcmp(name, props.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if(!found) return false;
    }
    return true;
}

void Vulkan::run() {
    createInstance();
    if(ENABLE_VALIDATION) setupDebugMessenger();
    pickPhysicalDevice();
    cleanup();
}
void Vulkan::createInstance() {
    if(ENABLE_VALIDATION) {
        if(!supportsLayers(VALIDATION_LAYERS)) {
            throw std::runtime_error("Requested Layers not available");
        }
        if(!supportsExtensions(EXTENSIONS)) {
            throw std::runtime_error("Requested Extensions not available");
        }
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Dithery Do";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = debugMessengerInfo();

    if(ENABLE_VALIDATION) {
        createInfo.enabledLayerCount = VALIDATION_LAYERS.size();
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        createInfo.enabledExtensionCount = EXTENSIONS.size();
        createInfo.ppEnabledExtensionNames = EXTENSIONS.data();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &messengerCreateInfo;
    }

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    EXPECT(result == VK_SUCCESS, "Failed to create Vulkan Instance")

    m_instance = vk::Instance(instance);
}

void Vulkan::setupDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = debugMessengerInfo();

    VkDebugUtilsMessengerEXT messenger;
    vk::ext::createDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &messenger);
    m_debugMessenger = vk::DebugUtilsMessengerEXT(messenger);
}
bool Vulkan::isDeviceSuitable(vk::PhysicalDevice device) const {
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.isComplete();
}
void Vulkan::pickPhysicalDevice() {
    std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
    EXPECT(!devices.empty(), "No GPUs found with Vulkan support");

    m_physicalDevice = VK_NULL_HANDLE;
    for (const auto& device : devices) {
        if(isDeviceSuitable(device)) {
            m_physicalDevice = device;
        }
    }
    EXPECT(m_physicalDevice, "No suitable GPUs found")
}


void Vulkan::cleanup() {
    vk::ext::destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    m_instance.destroy();
}
