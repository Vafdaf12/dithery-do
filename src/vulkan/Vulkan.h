#pragma once

#include "./util/Instance.h"
#include "util/MemoryPool.h"

#include <vector>
#include <vulkan/vulkan_core.h>

class Vulkan {
public:
    void run();

private:
    static constexpr uint32_t WIDTH = 200;
    static constexpr uint32_t HEIGHT = 100;

    bool isDeviceSuitable(VkPhysicalDevice device) const;

    // Initial setup
    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();

    void createSourceImage();
    void createCommandPool();
    void createComputePipeline();

    void cleanup();

    void transitionImageLayout(VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkCommandBuffer cmdBuffer);

    VkCommandBuffer beginTransferBuffer() const;
    void submitTransferBuffer(VkCommandBuffer buffer);

    vk::ext::Instance m_instance;
    vk::ext::DevicePool m_memoryPool;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;

    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;

    VkQueue m_computeQueue;
    VkQueue m_transferQueue;

    VkCommandPool m_transferCommandPool;
    VkCommandPool m_computeCommandPool;

    VkDeviceSize m_imageSize;

    VkImage m_sourceImage;
    VkImageView m_sourceImageView;
    VkDeviceMemory m_sourceImageMemory;
};
