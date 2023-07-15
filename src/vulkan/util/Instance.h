#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace vk::ext {
class Instance {
public:
    Instance() = default;
    Instance(VkApplicationInfo appInfo,
        std::vector<const char*> extensions = {},
        std::vector<const char*> layers = {});
    ~Instance();

    // Move semantics prevent incorrect destruction of instance
    Instance(Instance&& other);
    Instance& operator=(Instance&& other);

    // Instances may not be copied
    Instance(Instance&) = delete;
    Instance& operator=(Instance&) = delete;

    // Compatbility with native types
    inline operator VkInstance() const { return m_instance; }
    inline operator bool() const { return m_instance != VK_NULL_HANDLE; }

private:
    VkInstance m_instance = VK_NULL_HANDLE;

#ifndef NDEBUG
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
#endif
};

} // namespace vk::ext
