#include "./vulkan_wrapper.h"


#include <cstdint>
#include <fstream>
#include <ios>
#include <vulkan/vulkan_core.h>

namespace vk {
std::vector<VkLayerProperties> getInstanceLayers() {
    uint32_t count;
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    std::vector<VkLayerProperties> properties(count);
    vkEnumerateInstanceLayerProperties(&count, properties.data());

    return properties;
}
std::vector<VkExtensionProperties> getInstanceExtensions() {
    uint32_t count;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> properties(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, properties.data());

    return properties;
}
std::vector<VkExtensionProperties> getDeviceExtensions(
    VkPhysicalDevice device) {
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> properties(count);
    vkEnumerateDeviceExtensionProperties(
        device, nullptr, &count, properties.data());

    return properties;
}
std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance) {
    uint32_t count;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());

    return devices;
}
std::vector<VkQueueFamilyProperties> getQueueFamilies(
    VkPhysicalDevice physicalDevice) {
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

    std::vector<VkQueueFamilyProperties> arr(count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &count, arr.data());

    return arr;
}
std::vector<VkSurfaceFormatKHR> getDeviceSurfaceFormats(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);

    std::vector<VkSurfaceFormatKHR> arr(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, arr.data());

    return arr;
}
std::vector<VkPresentModeKHR> getDevicePresentationModes(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);

    std::vector<VkPresentModeKHR> arr(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &count, arr.data());

    return arr;
}
std::vector<VkImage> getSwapChainImages(
    VkDevice device, VkSwapchainKHR swapChain) {
    uint32_t count;
    vkGetSwapchainImagesKHR(device, swapChain, &count, nullptr);

    std::vector<VkImage> arr(count);
    vkGetSwapchainImagesKHR(device, swapChain, &count, arr.data());

    return arr;
}

VkResult createDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");

    if (!func) return VK_ERROR_EXTENSION_NOT_PRESENT;
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
}
void destroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func) func(instance, debugMessenger, pAllocator);
}
VkShaderModule createShaderModule(
    VkDevice device, const std::vector<char>& source) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(source.data());
    createInfo.codeSize = source.size();

    VkShaderModule module;
    VkResult result =
        vkCreateShaderModule(device, &createInfo, nullptr, &module);
    EXPECT(result == VK_SUCCESS, "Failed to create shader module")
    return module;
}
VkSemaphore createSemaphore(VkDevice device) {
    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphore;
    VkResult result =
        vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
    EXPECT(result == VK_SUCCESS, "Failed to create semaphore");

    return semaphore;
}
VkFence createFence(VkDevice device, bool signaled) {
    VkFenceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VkFence fence;
    VkResult result = vkCreateFence(device, &createInfo, nullptr, &fence);
    EXPECT(result == VK_SUCCESS, "Failed to create fence");

    return fence;
}
std::vector<VkCommandBuffer> allocateCommandBuffers(
    VkDevice device, VkCommandPool pool, uint32_t count, bool secondary) {
    std::vector<VkCommandBuffer> buffers(count);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    VkResult res = vkAllocateCommandBuffers(device, &allocInfo, buffers.data());
    EXPECT(res == VK_SUCCESS, "Failed to create command buffers");

    return buffers;
}
} // namespace vk
namespace util {
std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    EXPECT(file.is_open(), "Failed to open file")

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}
} // namespace util
