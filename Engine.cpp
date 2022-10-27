#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <libpng16/png.h>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <iomanip>

#include <chrono>
#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <algorithm>

#include <vector>
#include <cstring>
#include <limits>
#include <optional>
#include <set>
#include <array>

struct Engine
{
    const int width;
    const int height;

    const int framesInFlight;

    VkInstance instance;
    VkSurfaceKHR surface;
    GLFWwindow* window;
    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VkDebugUtilsMessengerEXT debugMessenger;
    VkViewport viewport;
    VkRect2D scissor;

    const std::vector<const char *> validationLayers;
    const std::vector<const char *> deviceExtensions;
};

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}


VkResult CreateDebugUtils(Engine engine, PFN_vkDebugUtilsMessengerCallbackEXT callback = DebugCallback)
{
    VkDebugUtilsMessengerCreateInfoEXT CreateInfo;

    CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    CreateInfo.pfnUserCallback = callback;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(engine.instance, "vkCreateDebugUtilsMessengerEXT");

    auto result = (func != nullptr) ? func(engine.instance, &CreateInfo, nullptr, &(engine.debugMessenger)) : VK_ERROR_EXTENSION_NOT_PRESENT;

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void DestroyDebugUtils(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}