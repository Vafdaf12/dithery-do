#pragma once

#include "MemoryPool.h"

#include <vulkan/vulkan_core.h>

namespace vk::ext {
class StagingBuffer {
public:
    StagingBuffer(DevicePool& bufferPool, VkDeviceSize size);
    ~StagingBuffer();

    void copyToBuffer(
        VkBuffer buffer, VkCommandBuffer cmdBuffer);
    void copyToImage(VkImage image,
        uint32_t width,
        uint32_t height,
        VkCommandBuffer cmdBuffer);
    void write(const void* data);

private:

    void* m_pMapped;
    VkDeviceSize m_size;
    VkDevice m_device;
    VkBuffer m_buffer;
    VkDeviceMemory m_memory;
};
} // namespace vk::ext
