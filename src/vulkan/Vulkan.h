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

    void createInstance();
    void setupDebugMessenger();
    void cleanup();

    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
};
