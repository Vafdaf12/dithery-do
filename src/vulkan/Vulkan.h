#pragma once

#include "./util/Instance.h"
#include "util/MemoryPool.h"

#include <vector>
#include <vulkan/vulkan_core.h>

class Vulkan {
public:
    void run();

private:
    static int WIDTH;
    static int HEIGHT;

    bool isDeviceSuitable(VkPhysicalDevice device) const;

    // Initial setup
    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();

    void createSourceImage();
    void createDestImage();
    void createStagingBuffer();
    void createCommandPool();
    void createComputePipeline();
    std::vector<uint8_t> readDestImage();

    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();

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

    VkImage m_destImage;
    VkImageView m_destImageView;
    VkDeviceMemory m_destImageMemory;

    VkBuffer m_stagingBuffer;
    VkDeviceMemory m_stagingMemory;

    VkDescriptorSetLayout m_descriptorSetLayout;
    VkDescriptorPool m_descriptorPool;
    VkDescriptorSet m_descriptorSet;
};
