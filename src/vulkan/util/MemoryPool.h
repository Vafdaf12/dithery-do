#pragma once

#include <stack>
#include <vulkan/vulkan_core.h>

namespace vk::ext {

class DevicePool {
public:
    DevicePool() = default;
    DevicePool(VkPhysicalDevice physicalDevice, VkDevice device);
    ~DevicePool();

    VkResult createBuffer(VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkBuffer& buffer,
        bool include = true);

    VkResult createImage(uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkImage& image,
        bool include = true);

    VkResult createImageView(
        VkImage image, VkFormat format, VkImageView& view, bool include = true);

    VkResult allocBuffer(VkBuffer buffer,
        VkMemoryPropertyFlags properties,
        VkDeviceMemory& memory, bool include = true);

    VkResult allocImage(VkImage image,
        VkMemoryPropertyFlags properties,
        VkDeviceMemory& memory, bool include = true);

    VkResult allocateRaw(VkMemoryRequirements requirements,
        VkMemoryPropertyFlags properties,
        VkDeviceMemory& memory) const;

    void destroy();
    inline operator bool() const {
        return m_device != VK_NULL_HANDLE && m_physicalDevice != VK_NULL_HANDLE;
    }

    inline VkDevice device() const { return m_device; }

    uint32_t findMemoryType(
        uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
    std::stack<VkBuffer> m_buffers;
    std::stack<VkImage> m_images;
    std::stack<VkImageView> m_imageViews;
    std::stack<VkDeviceMemory> m_deviceMemory;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
};
} // namespace vk::ext
