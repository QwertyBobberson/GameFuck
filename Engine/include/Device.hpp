#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "HelperStructs.hpp"

class Device
{
public:
    static VkPhysicalDevice physicalDevice;
    static VkFormat FindSupportedFormat(const std::vector<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    static unsigned int FindMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);
    static VkFormat FindDepthFormat();
    static bool hasStencilComponent(VkFormat format);
    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static int isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const std::vector<const char *> deviceExtensions);
    static bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char *> deviceExtensions);
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
};
