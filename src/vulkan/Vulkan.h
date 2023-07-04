#pragma once

#include <vulkan/vulkan.h>

class Vulkan {
public:
    void run();

private:
    VkInstance m_instance;
};
