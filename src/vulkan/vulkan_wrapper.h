#pragma once

#include <vulkan/vulkan_core.h>

#include <stdexcept>
#include <vector>

#define EXPECT(cond, msg)                                                      \
    if (!(cond)) throw std::runtime_error(msg);

#define EXPECT_VK(result, msg)                                                      \
    if ((result) != VK_SUCCESS) throw std::runtime_error(msg);

#define DESTROY(type, func)                                                    \
    template <>                                                                \
    void destroy<type>(VkDevice device, type obj) {                            \
        func(device, obj, nullptr);                                            \
    }

namespace vk {
std::vector<VkLayerProperties> getInstanceLayers();
std::vector<VkExtensionProperties> getInstanceExtensions();
std::vector<VkExtensionProperties> getDeviceExtensions(VkPhysicalDevice device);

std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance);
std::vector<VkQueueFamilyProperties> getQueueFamilies(
    VkPhysicalDevice physicalDevice);

std::vector<VkSurfaceFormatKHR> getDeviceSurfaceFormats(
    VkPhysicalDevice device, VkSurfaceKHR surface);
std::vector<VkPresentModeKHR> getDevicePresentationModes(
    VkPhysicalDevice device, VkSurfaceKHR surface);

std::vector<VkImage> getSwapChainImages(
    VkDevice device, VkSwapchainKHR swapChain);

VkResult createDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void destroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator);
VkShaderModule createShaderModule(
    VkDevice device, const std::vector<char>& source);
VkSemaphore createSemaphore(VkDevice device);
VkFence createFence(VkDevice device, bool signaled);
std::vector<VkCommandBuffer> allocateCommandBuffers(VkDevice device,
    VkCommandPool pool,
    uint32_t count,
    bool secondary = false);

template<class T>
void destroy(VkDevice device, T object);
} // namespace vk

namespace util {
std::vector<char> readFile(const std::string& filename);
}
