#include "MemoryPool.h"

#include "../vulkan_wrapper.h"

#include <vulkan/vulkan_core.h>

namespace vk::ext {

MemoryPool::MemoryPool(VkPhysicalDevice physicalDevice, VkDevice device)
    : m_physicalDevice(physicalDevice), m_device(device) {}

MemoryPool::~MemoryPool() { destroy(); }

void MemoryPool::destroy() {
    while (!m_buffers.empty()) {
        vkDestroyBuffer(m_device, m_buffers.top(), nullptr);
        m_buffers.pop();
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

void MemoryPool::createBuffer(VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& memory) {

    VkResult result = createBufferRaw(size, usage, properties, buffer, memory);
    EXPECT_VK(result, "Failed to create + allocate buffer");

    m_buffers.push(buffer);
    m_deviceMemory.push(memory);
}

VkResult MemoryPool::createBufferRaw(VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& memory) const {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult res = vkCreateBuffer(m_device, &createInfo, nullptr, &buffer);
    if (res != VK_SUCCESS) return res;

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_device, buffer, &memRequirements);

    res = allocateMemory(memRequirements, properties, memory);
    if (res != VK_SUCCESS) return res;

    return vkBindBufferMemory(m_device, buffer, memory, 0);
}

void MemoryPool::createImage(uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory) {

    VkResult result = createImageRaw(
        width, height, format, tiling, usage, properties, image, imageMemory);
    EXPECT_VK(result, "Failed to create + allocate image");

    m_images.push(image);
    m_deviceMemory.push(imageMemory);
}
VkResult MemoryPool::createImageRaw(uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory) const {
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

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, image, &memRequirements);

    result = allocateMemory(memRequirements, properties, imageMemory);
    if (result != VK_SUCCESS) return result;

    return vkBindImageMemory(m_device, image, imageMemory, 0);
}

VkResult MemoryPool::allocateMemory(VkMemoryRequirements requirements,
    VkMemoryPropertyFlags props,
    VkDeviceMemory& memory) const {
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = requirements.size;
    allocInfo.memoryTypeIndex =
        findMemoryType(requirements.memoryTypeBits, props);

    return vkAllocateMemory(m_device, &allocInfo, nullptr, &memory);
}
uint32_t MemoryPool::findMemoryType(
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
