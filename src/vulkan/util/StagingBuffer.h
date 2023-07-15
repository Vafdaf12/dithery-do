#pragma once

#include "MemoryPool.h"

#include <vulkan/vulkan_core.h>

namespace vk::ext {
class StagingBuffer {
public:
    StagingBuffer(const MemoryPool& bufferPool, VkDeviceSize size);
    ~StagingBuffer();

    void copyToBuffer(
        VkBuffer buffer, VkQueue queue, VkCommandPool commandPool);
    void copyToImage(VkImage image,
        uint32_t width,
        uint32_t height,
        VkQueue queue,
        VkCommandPool commandPool);
    void write(const void* data);

private:
    VkCommandBuffer beginCommandBuffer(VkCommandPool pool);
    void submitCommandBuffer(
        VkCommandBuffer cmdBuffer, VkCommandPool pool, VkQueue queue);

    void* m_pMapped;
    VkDeviceSize m_size;
    VkDevice m_device;
    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
};
} // namespace vk::ext
