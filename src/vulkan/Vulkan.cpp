#include "Vulkan.h"

#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "vulkan_wrapper.h"
#include "vulkan_ext.h"

#ifdef NDEBUG
constexpr bool ENABLE_VALIDATION = false;
#else
constexpr bool ENABLE_VALIDATION = true;
#endif

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

void Vulkan::run() {
    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();
    cleanup();
}
void Vulkan::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Dithery Do";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    m_instance = vk::ext::Instance(appInfo);
}

bool Vulkan::isDeviceSuitable(VkPhysicalDevice device) const {
    auto indices = vk::ext::queryQueueFamilies(device);
    return indices.isComplete();
}
void Vulkan::pickPhysicalDevice() {
    std::vector<VkPhysicalDevice> devices = vk::getPhysicalDevices(m_instance);
    EXPECT(!devices.empty(), "No GPUs found with Vulkan support");

    m_physicalDevice = VK_NULL_HANDLE;
    for (const auto& device : devices) {
        if(isDeviceSuitable(device)) {
            m_physicalDevice = device;
        }
    }
    EXPECT(m_physicalDevice, "No suitable GPUs found")
}

void Vulkan::createLogicalDevice() {
    auto indices = vk::ext::queryQueueFamilies(m_physicalDevice);
    EXPECT(indices.isComplete(), "Missing queue families")

    float priority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.computeFamily.value();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &priority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;

    if(ENABLE_VALIDATION) {
        createInfo.enabledLayerCount = VALIDATION_LAYERS.size();
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    VkResult res = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    EXPECT(res == VK_SUCCESS, "Failed to create logical device")
    vkGetDeviceQueue(m_device, indices.computeFamily.value(), 0, &m_computeQueue);
}

void Vulkan::cleanup() {
    vkDestroyDevice(m_device, nullptr);
}
