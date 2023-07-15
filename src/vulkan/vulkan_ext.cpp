#include "vulkan_ext.h"

#include <set>
#include <vulkan/vulkan_core.h>

#include "vulkan_wrapper.h"

namespace vk::ext {
QueueFamilyIndices queryQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  std::vector<VkQueueFamilyProperties> queueFamilies;
  queueFamilies = vk::getQueueFamilies(device);
  int i = 0;
  for (const auto &family : queueFamilies) {
    if (family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      indices.computeFamily = i;
    }
    if(family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      indices.transferFamily = i;
    }

    i++;
  }

  return indices;
}
bool instanceLayersSupported(const std::vector<const char *> &layers) {
  std::vector<VkLayerProperties> availableLayers;
  availableLayers = vk::getInstanceLayers();

  std::set<std::string> required(layers.begin(), layers.end());
  for (const auto &ext : availableLayers) {
    required.erase(ext.layerName);
  }
  return required.empty();
}

bool instanceExtensionsSupported(const std::vector<const char *> &extensions) {
  std::vector<VkExtensionProperties> availableExtensions;
  availableExtensions = vk::getInstanceExtensions();

  std::set<std::string> required(extensions.begin(), extensions.end());
  for (const auto &ext : availableExtensions) {
    required.erase(ext.extensionName);
  }
  return required.empty();
}
bool deviceExtensionsSupported(VkPhysicalDevice device,
                               const std::vector<const char *> &extensions) {
  std::vector<VkExtensionProperties> available =
      vk::getDeviceExtensions(device);

  std::set<std::string> required(extensions.begin(), extensions.end());
  for (const auto &ext : available) {
    required.erase(ext.extensionName);
  }
  return required.empty();
}
VkViewport viewportFromExtent(VkExtent2D extent) {
  VkViewport viewport{};
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  return viewport;
}
VkRect2D rectFromExtent(VkExtent2D extent) {
  VkRect2D rect;
  rect.offset = {0, 0};
  rect.extent = extent;
  return rect;
}
VkCommandPool createCommandPool(VkDevice device, uint32_t queueFamily,
                                VkCommandPoolCreateFlags flags) {

  VkCommandPoolCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  createInfo.flags = flags;
  createInfo.queueFamilyIndex = queueFamily;

  VkCommandPool pool;
  VkResult res = vkCreateCommandPool(device, &createInfo, nullptr, &pool);
  EXPECT(res == VK_SUCCESS, "Failed to create command pool")
  return pool;
}

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format) {
  VkImageViewCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = image;

  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = format;

  createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;

  VkImageView view;
  VkResult res = vkCreateImageView(device, &createInfo, nullptr, &view);
  EXPECT_VK(res, "Failed to create image view")
  return view;
}
} // namespace vk::ext
