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
    void createComputePipeline();
    void pickPhysicalDevice();
    void createLogicalDevice();

    void createCommandPool();
    void createImages();

    void cleanup();

    vk::ext::Instance m_instance;
    vk::ext::MemoryPool m_memoryPool;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;

    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;

    VkQueue m_computeQueue;
    VkQueue m_transferQueue;

    VkCommandPool m_transferCommandPool;
    VkCommandPool m_computeCommandPool;

    VkImage m_destImage;
    VkImageView m_destImageView;
    VkDeviceMemory m_destImageMemory;
};
