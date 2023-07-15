#pragma once

#include <vulkan/vulkan_core.h>

#include <array>
#include <optional>
#include <vector>

namespace vk::ext {
struct QueueFamilyIndices {
  std::optional<uint32_t> computeFamily;

  inline bool isComplete() const {
      return computeFamily.has_value();
  }
};

QueueFamilyIndices queryQueueFamilies(VkPhysicalDevice device);

bool instanceLayersSupported(const std::vector<const char *> &layers);
bool instanceExtensionsSupported(const std::vector<const char *> &extensions);
bool deviceExtensionsSupported(VkPhysicalDevice device,
                               const std::vector<const char *> &extensions);
VkViewport viewportFromExtent(VkExtent2D extent);
VkRect2D rectFromExtent(VkExtent2D extent);

VkCommandPool createCommandPool(VkDevice device, uint32_t queueFamily,
                                VkCommandPoolCreateFlags flags);

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format);
} // namespace vk::ext
