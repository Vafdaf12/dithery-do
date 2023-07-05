#pragma once

#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.hpp>

class Vulkan {
public:
    void run();

private:
    bool supportsLayers(const std::vector<const char*>& layers) const;
    bool supportsExtensions(const std::vector<const char*>& layers) const;
    bool isDeviceSuitable(vk::PhysicalDevice device) const;

    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void cleanup();

    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_device;
    vk::Queue m_computeQueue;
};
