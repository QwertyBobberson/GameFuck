#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>

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

#include "CommandPool.hpp"
#include "Device.hpp"
#include "HelperStructs.hpp"
#include "Input.hpp"

struct Engine
{
    static Engine* engine;
    CommandPool* cmdPool;


    VkDebugUtilsMessengerEXT debugMessenger;
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};

	const unsigned int MaxFramesInFlight;

    GLFWwindow* window;
    VkInstance instance;

    SwapChainSupportDetails swapChainSupport;

    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

    Engine(const char* applicationName, unsigned int width, unsigned int height, GLFWframebuffersizefun framebufferResizeCallback = nullptr, int(*ScoreDevice)(VkPhysicalDevice, VkSurfaceKHR, const std::vector<const char *>) = nullptr, int framesInFlight = 3);

private:
    void SetupWindow(const char* applicationName, GLFWframebuffersizefun framebufferResizeCallback, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback);
	void PickPhysicalDevice(int(*ScoreDevice)(VkPhysicalDevice, VkSurfaceKHR, const std::vector<const char *>));
    void CreateLogicalDevice();

    void SetupDebugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT debugCallback);
    std::vector<const char*> GetRequiredExtensions();
    bool CheckValidationLayerSupport();

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities);

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
};
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);