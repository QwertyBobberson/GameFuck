#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/glfw3.h>
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

#include "Helpers.hpp"

struct Engine
{
    VkDebugUtilsMessengerEXT debugMessenger;
    const std::vector<const char *> validationLayers  = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    const unsigned int width;
    const unsigned int height;
	const unsigned int MaxFramesInFlight;

    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDevice device;
    VkPhysicalDevice physicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

    Engine(const char* applicationName, int _width, int _height, int framesInFlight = 3, GLFWframebuffersizefun framebufferResizeCallback = FramebufferResizeCallback, int(*ScoreDevice)(VkPhysicalDevice, VkSurfaceKHR, const std::vector<const char *>) = isDeviceSuitable);

    void SetupWindow(const char* applicationName, GLFWframebuffersizefun framebufferResizeCallback, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback);

	void PickPhysicalDevice(int(*ScoreDevice)(VkPhysicalDevice, VkSurfaceKHR, const std::vector<const char *>));

    void CreateLogicalDevice();

private:
    void SetupDebugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT debugCallback);
};