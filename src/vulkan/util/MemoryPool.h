#pragma once

#include <stack>
#include <vulkan/vulkan_core.h>

namespace vk::ext {

class MemoryPool {
public:
    MemoryPool() = default;
    MemoryPool(VkPhysicalDevice physicalDevice, VkDevice device);
    ~MemoryPool();

    void createBuffer(VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& memory);

    VkResult createBufferRaw(VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& memory) const;

    void createImage(uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory);

    VkResult createImageRaw(uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory) const;

    void destroy();
    inline operator bool() const {
        return m_device != VK_NULL_HANDLE && m_physicalDevice != VK_NULL_HANDLE;
    }

    inline VkDevice device() const { return m_device; }

    uint32_t findMemoryType(
        uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
    VkResult allocateMemory(VkMemoryRequirements requirements,
        VkMemoryPropertyFlags props,
        VkDeviceMemory& memory) const;

    std::stack<VkBuffer> m_buffers;
    std::stack<VkImage> m_images;
    std::stack<VkDeviceMemory> m_deviceMemory;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
};
} // namespace vk::ext
