#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

class Vulkan {
public:
    void run();

private:
    bool supportsLayers(const std::vector<const char*>& layers) const;
    bool supportsExtensions(const std::vector<const char*>& layers) const;

    void createInstance();
    void cleanup();

    vk::Instance m_instance;
};
