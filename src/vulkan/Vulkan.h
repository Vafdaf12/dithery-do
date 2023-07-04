#pragma once

#include <vulkan/vulkan.h>

class Vulkan {
public:
    void run();

private:
    void createInstance();
    void cleanup();

    VkInstance m_instance;
};
