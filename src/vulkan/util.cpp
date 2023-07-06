#include "util.h"
#include <vulkan/vulkan_core.h>

namespace vk {
    std::vector<VkLayerProperties> getInstanceLayers() {
        uint32_t count;
        vkEnumerateInstanceLayerProperties(&count, nullptr);
        
        std::vector<VkLayerProperties> properties(count);
        vkEnumerateInstanceLayerProperties(&count, properties.data());
        
        return properties;
    }
    std::vector<VkExtensionProperties> getInstanceExtensions() {
        uint32_t count;
        vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        
        std::vector<VkExtensionProperties> properties(count);
        vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());
        
        return properties;
    }
    std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance) {
        uint32_t count;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);
        
        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(instance, &count, devices.data());
        
        return devices;
    }
    std::vector<VkQueueFamilyProperties> getQueueFamilies(VkPhysicalDevice physicalDevice) {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
        
        std::vector<VkQueueFamilyProperties> arr(count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, arr.data());
        
        return arr;
    }
}
