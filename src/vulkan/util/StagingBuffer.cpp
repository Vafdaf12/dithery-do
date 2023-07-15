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
    VkBuffer buffer, VkQueue queue, VkCommandPool commandPool) {
    VkCommandBuffer cmdBuffer = beginCommandBuffer(commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.size = m_size;
    vkCmdCopyBuffer(cmdBuffer, m_buffer, buffer, 1, &copyRegion);

    submitCommandBuffer(cmdBuffer, commandPool, queue);
}
void StagingBuffer::copyToImage(VkImage image,
    uint32_t width,
    uint32_t height,
    VkQueue queue,
    VkCommandPool commandPool) {
    VkCommandBuffer cmdBuffer = beginCommandBuffer(commandPool);

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

    submitCommandBuffer(cmdBuffer, commandPool, queue);
}
VkCommandBuffer StagingBuffer::beginCommandBuffer(VkCommandPool pool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(m_device, &allocInfo, &cmdBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmdBuffer, &beginInfo);
    return cmdBuffer;
}
void StagingBuffer::submitCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandPool pool, VkQueue queue) {
    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(m_device, pool, 1, &cmdBuffer);
}
void StagingBuffer::write(const void* pData) {
    memcpy(m_pMapped, pData, m_size);
}
} // namespace vk::ext
