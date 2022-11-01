#pragma once
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

#include "HelperFunctions.cpp"

struct Engine
{
    VkDebugUtilsMessengerEXT debugMessenger;
    const std::vector<const char *> validationLayers;
    bool enableValidationLayers;
    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    const int width;
    const int height;
	const int MaxFramesInFlight;

    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDevice device;
    VkPhysicalDevice physicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

    const std::vector<const char *> deviceExtensions;

    void SetupWindow(char* applicationName = "", bool useValidationLayers = false)
    {
        enableValidationLayers = useValidationLayers;
        // if (enableValidationLayers && !CheckValidationLayerSupport(*this))
        // {
        //     throw std::runtime_error("validation layers requested, but not available!");
        // }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = applicationName;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        CreateInfo.pApplicationInfo = &appInfo;

        auto extensions = GetRequiredExtensions(enableValidationLayers);
        CreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        CreateInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        // if (enableValidationLayers)
        // {
        //     CreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        //     CreateInfo.ppEnabledLayerNames = validationLayers.data();

        //     PopulateDebugMessengerCreateInfo(debugCreateInfo);
        //     CreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        // }
        // else
        {
            CreateInfo.enabledLayerCount = 0;

            CreateInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&CreateInfo, nullptr, &(instance)) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to Create instance!");
        }

		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to Create window surface!");
		}
    }


	void PickPhysicalDevice(int(*ScoreDevice)(Engine*) = isDeviceSuitable)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        VkPhysicalDevice bestDevice;
        int highestScore = 0;
		for (const auto &device : devices)
		{
            int score = ScoreDevice(this);
			if (score > highestScore)
			{
				highestScore = score;
				bestDevice = device;
			}
		}

		if (bestDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}

        physicalDevice = bestDevice;
	}

    void CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(this);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		CreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		CreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		CreateInfo.pEnabledFeatures = &deviceFeatures;

		CreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		CreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers)
		{
			CreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			CreateInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			CreateInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physicalDevice, &CreateInfo, nullptr, &device) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to Create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
};