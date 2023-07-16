#include "MemoryPool.h"

#include "../vulkan_wrapper.h"

#include <vulkan/vulkan_core.h>

namespace vk::ext {

DevicePool::DevicePool(VkPhysicalDevice physicalDevice, VkDevice device)
    : m_physicalDevice(physicalDevice), m_device(device) {}

DevicePool::~DevicePool() { destroy(); }

void DevicePool::destroy() {
    while (!m_buffers.empty()) {
        vkDestroyBuffer(m_device, m_buffers.top(), nullptr);
        m_buffers.pop();
    }
    while (!m_imageViews.empty()) {
        vkDestroyImageView(m_device, m_imageViews.top(), nullptr);
        m_imageViews.pop();
    }
    while (!m_images.empty()) {
        vkDestroyImage(m_device, m_images.top(), nullptr);
        m_images.pop();
    }

    while (!m_deviceMemory.empty()) {
        vkFreeMemory(m_device, m_deviceMemory.top(), nullptr);
        m_deviceMemory.pop();
    }
}

VkResult DevicePool::allocBuffer(
    VkBuffer buffer, VkMemoryPropertyFlags properties, VkDeviceMemory& memory, bool include) {

    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

    VkResult result = allocateRaw(memRequirements, properties, memory);
    if (result != VK_SUCCESS) return result;

    result = vkBindBufferMemory(m_device, buffer, memory, 0);
    if (result != VK_SUCCESS) return result;

    if(include) m_deviceMemory.push(memory);
    return result;
}
VkResult DevicePool::allocImage(
    VkImage image, VkMemoryPropertyFlags properties, VkDeviceMemory& memory, bool include) {

    VkMemoryRequirements memRequirements{};
    vkGetImageMemoryRequirements(m_device, image, &memRequirements);

    VkResult result = allocateRaw(memRequirements, properties, memory);
    if (result != VK_SUCCESS) return result;

    result = vkBindImageMemory(m_device, image, memory, 0);
    if (result != VK_SUCCESS) return result;

    if(include) m_deviceMemory.push(memory);
    return result;
}
VkResult DevicePool::createBuffer(
    VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, bool include) {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.usage = usage;

    VkResult res = vkCreateBuffer(m_device, &createInfo, nullptr, &buffer);
    if (res != VK_SUCCESS) return res;

    if(include) m_buffers.push(buffer);
    return res;
}
VkResult DevicePool::createImage(uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImage& image, bool include) {

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;

    VkResult result = vkCreateImage(m_device, &imageInfo, nullptr, &image);
    if (result != VK_SUCCESS) return result;

    if(include) m_images.push(image);
    return result;
}
VkResult DevicePool::createImageView(
    VkImage image, VkFormat format, VkImageView& view, bool include) 
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkResult res = vkCreateImageView(m_device, &createInfo, nullptr, &view);
    if(res != VK_SUCCESS) return res;

    if(include) m_imageViews.push(view);

    return res;
}

VkResult DevicePool::allocateRaw(VkMemoryRequirements requirements,
    VkMemoryPropertyFlags props,
    VkDeviceMemory& memory) const {
    VkMemoryAllocateInfo allocInfo{};

    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = requirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(requirements.memoryTypeBits, props);

    return vkAllocateMemory(m_device, &allocInfo, nullptr, &memory);
}
uint32_t DevicePool::findMemoryType(
    uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (!(typeFilter & (1 << i))) continue;
        if ((memProperties.memoryTypes[i].propertyFlags & properties) !=
            properties)
            continue;
        return i;
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

} // namespace vk::ext
