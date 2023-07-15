#pragma once

#include "./util/Instance.h"
#include "util/MemoryPool.h"

#include <vector>
#include <vulkan/vulkan_core.h>

class Vulkan {
public:
    void run();

private:
    bool isDeviceSuitable(VkPhysicalDevice device) const;

    // Initial setup
    void createInstance();
    void createComputePipeline();
    void pickPhysicalDevice();
    void createLogicalDevice();

    void createCommandPool();

    void cleanup();

    vk::ext::Instance m_instance;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_pipeline;

    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;

    VkQueue m_computeQueue;
    VkQueue m_transferQueue;

    VkCommandPool m_transferCommandPool;
    VkCommandPool m_computeCommandPool;
};
