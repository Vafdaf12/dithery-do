#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

namespace vk {
    std::vector<VkLayerProperties> getInstanceLayers();
    std::vector<VkExtensionProperties> getInstanceExtensions();
    std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance);
    std::vector<VkQueueFamilyProperties> getQueueFamilies(VkPhysicalDevice physicalDevice);
}
