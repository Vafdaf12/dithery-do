#include "StagingBuffer.h"

#include <cstring>
#include <vulkan/vulkan_core.h>

#include "../vulkan_wrapper.h"

namespace vk::ext {
StagingBuffer::StagingBuffer(const MemoryPool& bufferPool, VkDeviceSize size)
    : m_device(bufferPool.device()), m_size(size) {
    VkResult result = bufferPool.createBufferRaw(size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        m_buffer,
        m_memory);
    EXPECT_VK(result, "Failed tocreate + allocate staging buffer");

    result = vkMapMemory(m_device, m_memory, 0, m_size, 0, &m_pMapped);
    EXPECT_VK(result, "Failed to map staging buffer memory");
}

StagingBuffer::~StagingBuffer() {
    vkUnmapMemory(m_device, m_memory);
    vkDestroyBuffer(m_device, m_buffer, nullptr);
    vkFreeMemory(m_device, m_memory, nullptr);
}

void StagingBuffer::copyToBuffer(
    VkBuffer buffer, VkCommandBuffer cmdBuffer) {

    VkBufferCopy copyRegion{};
    copyRegion.size = m_size;
    vkCmdCopyBuffer(cmdBuffer, m_buffer, buffer, 1, &copyRegion);
}
void StagingBuffer::copyToImage(VkImage image,
    uint32_t width,
    uint32_t height,
    VkCommandBuffer cmdBuffer) {

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(cmdBuffer, m_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}
void StagingBuffer::write(const void* pData) {
    memcpy(m_pMapped, pData, m_size);
}
} // namespace vk::ext
