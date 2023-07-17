#include "Vulkan.h"

#include <array>
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
#include "../stb/stb_image_write.h"

#ifdef NDEBUG
constexpr bool ENABLE_VALIDATION = false;
#else
constexpr bool ENABLE_VALIDATION = true;
#endif

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"};

int Vulkan::WIDTH = 0;
int Vulkan::HEIGHT = 0;

void Vulkan::run() {
    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();

    m_memoryPool = vk::ext::DevicePool(m_physicalDevice, m_device);

    createCommandPool();
    createSourceImage();
    createDestImage();
    createStagingBuffer();

    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
    createComputePipeline();
    processImage();

    auto bytes = readDestImage();
    stbi_write_jpg("output.jpg", WIDTH, HEIGHT, 4, bytes.data(), 100);

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
    layoutInfo.pSetLayouts = &m_descriptorSetLayout;
    layoutInfo.setLayoutCount = 1;

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
VkCommandBuffer Vulkan::beginTransferBuffer() const {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_transferCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer buffer;
    VkResult result = vkAllocateCommandBuffers(m_device, &allocInfo, &buffer);
    EXPECT_VK(result, "Failed to allocate transfer")

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    result = vkBeginCommandBuffer(buffer, &beginInfo);
    EXPECT_VK(result, "Failed to begin transfer")

    return buffer;
}
void Vulkan::submitTransferBuffer(VkCommandBuffer buffer) {
    VkResult result = vkEndCommandBuffer(buffer);
    EXPECT_VK(result, "Failed to end transfer")

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    vkQueueSubmit(m_transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_transferQueue);

    vkFreeCommandBuffers(m_device, m_transferCommandPool, 1, &buffer);
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
void Vulkan::transitionImageLayout(VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkCommandBuffer cmdBuffer) {

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;

    // Indicates transfer of layout
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    // Not transferring queue family ownership
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    // What part of the image is affected by barrier
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    // Transfer from uninitialized layout to layout
    // optimized for writing to the image
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    // Transfer from uninitialized layout to layout
    // optimized for general use (destination image)
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_GENERAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    // Image has loaded and is ready to be accessed
    // by compute shader
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_GENERAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL &&
               newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else {
        throw std::invalid_argument("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(
        cmdBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Vulkan::createDestImage() {
    VkResult res;

    res = m_memoryPool.createImage(WIDTH,
        HEIGHT,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        m_destImage);
    EXPECT_VK(res, "Failed to create image")

    res = m_memoryPool.allocImage(
        m_destImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_destImageMemory);
    EXPECT_VK(res, "Failed to allocate image")

    res = m_memoryPool.createImageView(
        m_destImage, VK_FORMAT_R8G8B8A8_UNORM, m_destImageView);
    EXPECT_VK(res, "Failed to create image view")

    auto buffer = beginTransferBuffer();
    transitionImageLayout(m_destImage,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL,
        buffer);
    submitTransferBuffer(buffer);
}
void Vulkan::createStagingBuffer() {
    VkDeviceSize bufSize = WIDTH * HEIGHT * 4;

    VkResult res;
    res = m_memoryPool.createBuffer(
        bufSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, m_stagingBuffer);
    EXPECT_VK(res, "Failed to create staging buffer")

    res = m_memoryPool.allocBuffer(m_stagingBuffer,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        m_stagingMemory);
    EXPECT_VK(res, "Failed to create staging buffer memory")
}
std::vector<uint8_t> Vulkan::readDestImage() {
    VkDeviceSize bufSize = WIDTH * HEIGHT * 4;

    VkCommandBuffer buffer = beginTransferBuffer();
    transitionImageLayout(m_destImage,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        buffer);

    VkBufferImageCopy copyInfo{};
    copyInfo.bufferOffset = 0;
    copyInfo.bufferRowLength = 0;
    copyInfo.bufferImageHeight = 0;

    copyInfo.imageExtent = {
        static_cast<uint32_t>(WIDTH), static_cast<uint32_t>(HEIGHT), 1};
    copyInfo.imageOffset = {0, 0, 0};

    copyInfo.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyInfo.imageSubresource.mipLevel = 0;
    copyInfo.imageSubresource.baseArrayLayer = 0;
    copyInfo.imageSubresource.layerCount = 1;

    vkCmdCopyImageToBuffer(buffer,
        m_destImage,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        m_stagingBuffer,
        1,
        &copyInfo);

    submitTransferBuffer(buffer);

    std::vector<uint8_t> bytes(WIDTH * HEIGHT * 4);
    void* pData;
    vkMapMemory(m_device, m_stagingMemory, 0, bufSize, 0, &pData);
    memcpy(bytes.data(), pData, bufSize);

    vkUnmapMemory(m_device, m_stagingMemory);
    return bytes;
}
void Vulkan::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding inputBinding{};
    inputBinding.binding = 0;
    inputBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    inputBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    inputBinding.descriptorCount = 1;

    VkDescriptorSetLayoutBinding outputBinding{};
    outputBinding.binding = 1;
    outputBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    outputBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    outputBinding.descriptorCount = 1;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
        inputBinding, outputBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pBindings = bindings.data();
    layoutInfo.bindingCount = bindings.size();

    VkResult result = vkCreateDescriptorSetLayout(
        m_device, &layoutInfo, nullptr, &m_descriptorSetLayout);
    EXPECT_VK(result, "Failed to create descriptor set layout")
}
void Vulkan::createDescriptorPool() {
    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSize.descriptorCount = 2;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.poolSizeCount = 1;

    VkResult result =
        vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool);
    EXPECT_VK(result, "Failed to create descriptor pool")
}

VkWriteDescriptorSet imageInfo(VkDescriptorSet descriptorSet, uint32_t binding, const VkDescriptorImageInfo& imageInfo) {
    VkWriteDescriptorSet writeInfo{};
    writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeInfo.dstSet = descriptorSet;
    writeInfo.dstArrayElement = 0;
    writeInfo.dstBinding = binding;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeInfo.pImageInfo = &imageInfo;
    return writeInfo;
}
void Vulkan::createDescriptorSets() {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pSetLayouts = &m_descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;
    allocInfo.descriptorPool = m_descriptorPool;

    VkResult result =
        vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet);
    EXPECT_VK(result, "Failed to create descriptor pool");

    VkDescriptorImageInfo imageInfoDest{};
    imageInfoDest.sampler = VK_NULL_HANDLE;
    imageInfoDest.imageView = m_destImageView;
    imageInfoDest.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkDescriptorImageInfo imageInfoSource{};
    imageInfoSource.sampler = VK_NULL_HANDLE;
    imageInfoSource.imageView = m_sourceImageView;
    imageInfoSource.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet writeInfoDest = imageInfo(m_descriptorSet, 1, imageInfoDest);
    VkWriteDescriptorSet writeInfoSource = imageInfo(m_descriptorSet, 0, imageInfoSource);

    VkWriteDescriptorSet writes[] = {
        writeInfoSource, writeInfoDest
    };

    vkUpdateDescriptorSets(m_device, 2, writes, 0, nullptr);
}
void Vulkan::processImage() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_computeCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer buffer;
    vkAllocateCommandBuffers(m_device, &allocInfo, &buffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(buffer, &beginInfo);

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);
    vkCmdBindDescriptorSets(buffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        m_pipelineLayout,
        0,
        1,
        &m_descriptorSet,
        0,
        nullptr);
    vkCmdDispatch(buffer, WIDTH, HEIGHT, 1);

    vkEndCommandBuffer(buffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    vkQueueSubmit(m_computeQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_computeQueue);

}
void Vulkan::createSourceImage() {
    int n;
    uint8_t* pPixels = stbi_load("res/robot.png", &WIDTH, &HEIGHT, &n, 4);
    m_imageSize = WIDTH * HEIGHT * 4;
    EXPECT(pPixels, "Failed to load image")

    VkResult res;
    res = m_memoryPool.createImage(WIDTH,
        HEIGHT,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        m_sourceImage);
    EXPECT_VK(res, "Failed to create image")

    res = m_memoryPool.allocImage(m_sourceImage,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_sourceImageMemory);
    EXPECT_VK(res, "Failed to allocate image")

    res = m_memoryPool.createImageView(
        m_sourceImage, VK_FORMAT_R8G8B8A8_UNORM, m_sourceImageView);
    EXPECT_VK(res, "Failed to create image view")

    VkCommandBuffer cmdBuffer = beginTransferBuffer();

    transitionImageLayout(m_sourceImage,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        cmdBuffer);

    vk::ext::StagingBuffer stagingBuffer(m_memoryPool, m_imageSize);
    stagingBuffer.write(pPixels);
    stagingBuffer.copyToImage(m_sourceImage, WIDTH, HEIGHT, cmdBuffer);

    transitionImageLayout(m_sourceImage,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        cmdBuffer);

    submitTransferBuffer(cmdBuffer);
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
        m_device, indices.transferFamily.value(), 0, &m_transferQueue);
}

void Vulkan::cleanup() {
    vk::destroy(m_device, m_descriptorSetLayout);
    vk::destroy(m_device, m_descriptorPool);

    vk::destroy(m_device, m_pipeline);
    vk::destroy(m_device, m_pipelineLayout);

    m_memoryPool.destroy();
    vk::destroy(m_device, m_computeCommandPool);
    vk::destroy(m_device, m_transferCommandPool);
    vkDestroyDevice(m_device, nullptr);
}
