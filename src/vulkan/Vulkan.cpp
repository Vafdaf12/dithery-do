#include "Vulkan.h"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>

#ifdef NDEBUG
constexpr bool ENABLE_VALIDATION = false;
#else
constexpr bool ENABLE_VALIDATION = true;
#endif


const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char*> EXTENSIONS = {
#ifndef NDEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
};

bool Vulkan::supportsLayers(const std::vector<const char*>& layers) const {
    std::vector<vk::LayerProperties> availableLayers;
    availableLayers = vk::enumerateInstanceLayerProperties();

    // check if all layers are supported
    // TODO: refactor to use C++ STL algorithms
    for(const char* name: layers) {
        bool found = false;
        for(const auto& props : availableLayers) {
            if(strcmp(name, props.layerName) == 0) {
                found = true;
                break;
            }
        }
        if(!found) return false;
    }
    return true;
}

bool Vulkan::supportsExtensions(const std::vector<const char*>& layers) const {
    std::vector<vk::ExtensionProperties> availableExtensions;
    availableExtensions = vk::enumerateInstanceExtensionProperties();

    // check if all layers are supported
    // TODO: refactor to use C++ STL algorithms
    for(const char* name: layers) {
        bool found = false;
        for(const auto& props : availableExtensions) {
            if(strcmp(name, props.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if(!found) return false;
    }
    return true;
}

void Vulkan::run() {
    createInstance();
    cleanup();
}
void Vulkan::createInstance() {
    if(ENABLE_VALIDATION) {
        if(!supportsLayers(VALIDATION_LAYERS)) {
            throw std::runtime_error("Requested Layers not available");
        }
        if(!supportsExtensions(EXTENSIONS)) {
            throw std::runtime_error("Requested Extensions not available");
        }
    }

    vk::ApplicationInfo appInfo;
    appInfo.pApplicationName = "Dithery Do";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    vk::InstanceCreateInfo createInfo({}, &appInfo);

    if(ENABLE_VALIDATION) {
        createInfo.enabledLayerCount = VALIDATION_LAYERS.size();
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        createInfo.enabledExtensionCount = EXTENSIONS.size();
        createInfo.ppEnabledExtensionNames = EXTENSIONS.data();
    }

    vk::Result result = vk::createInstance(&createInfo, nullptr, &m_instance);
    if(result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create Vulkan Instance");
    }
}
void Vulkan::cleanup() {
    m_instance.destroy();
}
