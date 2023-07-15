#include "Vulkan.h"

#include <cstring>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan_core.h>

#include "util/StagingBuffer.h"
#include "vulkan_ext.h"
#include "vulkan_wrapper.h"

#include "../stb/stb_image.h"

#ifdef NDEBUG
constexpr bool ENABLE_VALIDATION = false;
#else
constexpr bool ENABLE_VALIDATION = true;
#endif

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"};

void Vulkan::run() {
    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();

    m_memoryPool = vk::ext::MemoryPool(m_physicalDevice, m_device);

    createCommandPool();
    createImages();

    cleanup();
}
void Vulkan::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Dithery Do";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    m_instance = vk::ext::Instance(appInfo);
}

void Vulkan::createComputePipeline() {
    auto src = util::readFile("res/comp.spv");
    auto module = vk::createShaderModule(m_device, src);

    VkPipelineShaderStageCreateInfo shaderStage{};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStage.module = module;
    shaderStage.pName = "main";

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    VkResult result = vkCreatePipelineLayout(
        m_device, &layoutInfo, nullptr, &m_pipelineLayout);
    EXPECT_VK(result, "Failed to create pipeline layout")

    VkComputePipelineCreateInfo computeInfo{};
    computeInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computeInfo.layout = m_pipelineLayout;
    computeInfo.basePipelineHandle = VK_NULL_HANDLE;
    computeInfo.basePipelineIndex = -1;
    computeInfo.stage = shaderStage;

    VkResult res = vkCreateComputePipelines(
        m_device, VK_NULL_HANDLE, 1, &computeInfo, nullptr, &m_pipeline);
    EXPECT_VK(res, "Failed to create compute pipeline");

    vkDestroyShaderModule(m_device, module, nullptr);
}
void Vulkan::createCommandPool() {
    auto families = vk::ext::queryQueueFamilies(m_physicalDevice);

    m_transferCommandPool = vk::ext::createCommandPool(m_device,
        families.transferFamily.value(),
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    m_computeCommandPool = vk::ext::createCommandPool(m_device,
        families.computeFamily.value(),
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}
void Vulkan::createImages() {
    m_memoryPool.createImage(WIDTH,
        HEIGHT,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_destImage,
        m_destImageMemory);

    m_destImageView = vk::ext::createImageView(
        m_device, m_destImage, VK_FORMAT_R8G8B8A8_UNORM);
}
bool Vulkan::isDeviceSuitable(VkPhysicalDevice device) const {
    auto indices = vk::ext::queryQueueFamilies(device);
    return indices.isComplete();
}
void Vulkan::pickPhysicalDevice() {
    std::vector<VkPhysicalDevice> devices = vk::getPhysicalDevices(m_instance);
    EXPECT(!devices.empty(), "No GPUs found with Vulkan support");

    m_physicalDevice = VK_NULL_HANDLE;
    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            m_physicalDevice = device;
        }
    }
    EXPECT(m_physicalDevice, "No suitable GPUs found")
}

void Vulkan::createLogicalDevice() {
    auto indices = vk::ext::queryQueueFamilies(m_physicalDevice);

    float priority = 1.0f;

    std::set<uint32_t> uniqueFamilies = {
        indices.computeFamily.value(), indices.transferFamily.value()};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t idx : uniqueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = idx;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pEnabledFeatures = &deviceFeatures;

    if (ENABLE_VALIDATION) {
        createInfo.enabledLayerCount = VALIDATION_LAYERS.size();
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    VkResult res =
        vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    EXPECT(res == VK_SUCCESS, "Failed to create logical device")

    vkGetDeviceQueue(
        m_device, indices.computeFamily.value(), 0, &m_computeQueue);
    vkGetDeviceQueue(
        m_device, indices.transferFamily.value(), 0, &m_computeQueue);
}

void Vulkan::cleanup() {
    vkDestroyImageView(m_device, m_destImageView, nullptr);

    m_memoryPool.destroy();
    vkDestroyCommandPool(m_device, m_computeCommandPool, nullptr);
    vkDestroyCommandPool(m_device, m_transferCommandPool, nullptr);
    vkDestroyDevice(m_device, nullptr);
}
