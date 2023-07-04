#include "Vulkan.h"
#include <iostream>
#include <stdexcept>


void Vulkan::run() {
    createInstance();
    cleanup();
}
void Vulkan::createInstance() {
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Dithery Do",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };
    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
    };

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan Instance");
    }
}
void Vulkan::cleanup() {
    vkDestroyInstance(m_instance, nullptr);
}
