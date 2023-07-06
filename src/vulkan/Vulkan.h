#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

class Vulkan {
public:
    void run();

private:
    bool supportsLayers(const std::vector<const char*>& layers) const;
    bool supportsExtensions(const std::vector<const char*>& layers) const;
    bool isDeviceSuitable(VkPhysicalDevice device) const;

    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void cleanup();

    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueue m_computeQueue;
};
