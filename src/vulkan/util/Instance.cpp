#include "Instance.h"

#include "../vulkan_wrapper.h"
#include "../vulkan_ext.h"

#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vk::ext {
Instance::Instance(VkApplicationInfo appInfo,
    std::vector<const char*> extensions,
    std::vector<const char*> layers) {

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
    if(!vk::ext::instanceExtensionsSupported(extensions)) {
        throw std::runtime_error("Requested instance extensions not supported");
    }
    if(!vk::ext::instanceLayersSupported(layers)) {
        throw std::runtime_error("Requested instance layers not supported");

    }

    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vulkan instance");
    }
    std::cout << "instance created" << std::endl;
    assert(m_instance != VK_NULL_HANDLE);
}
Instance::~Instance() {
    if(m_instance == VK_NULL_HANDLE) return;
    std::cout << "instance destroyed" << std::endl;

#ifndef NDEBUG
    vk::destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif
    vkDestroyInstance(m_instance, nullptr);
}
Instance::Instance(Instance&& other) {
    std::swap(m_instance, other.m_instance);

#ifndef NDEBUG
    std::swap(m_debugMessenger, other.m_debugMessenger);
#endif

}
Instance& Instance::operator=(Instance&& other) {
    std::swap(m_instance, other.m_instance);

#ifndef NDEBUG
    std::swap(m_debugMessenger, other.m_debugMessenger);
#endif
    return *this;
}

#ifndef NDEBUG
VkDebugUtilsMessengerCreateInfoEXT Instance::messengerCreateInfo() {
    static VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = debugCallback;
    return createInfo;
}
VKAPI_ATTR VkBool32 VKAPI_CALL Instance::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        return VK_FALSE;

    std::cout << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}
#endif
} // namespace vk::ext
