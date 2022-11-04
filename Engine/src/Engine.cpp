#pragma once
#include "../include/Engine.hpp"
#include "../include/Helpers.hpp"
#include "../include/Pipeline.hpp"
#include "RenderPass.cpp"
#include "Helpers.cpp"
#include "Pipeline.cpp"

Engine::Engine(const char* applicationName, int _width, int _height, int framesInFlight, GLFWframebuffersizefun framebufferResizeCallback, int(*ScoreDevice)(VkPhysicalDevice, VkSurfaceKHR, const std::vector<const char *>)) : width(_width), height(_height), MaxFramesInFlight(framesInFlight)
{
	SetupWindow(applicationName, framebufferResizeCallback, DebugCallback);
	PickPhysicalDevice(ScoreDevice);
	CreateLogicalDevice();
	#ifdef DEBUG
		SetupDebugMessenger(DebugCallback);
	#endif
}

void Engine::SetupWindow(const char* applicationName, GLFWframebuffersizefun framebufferResizeCallback, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, applicationName, nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	#ifdef DEBUG
	if (!CheckValidationLayerSupport(this))
	{
	    throw std::runtime_error("validation layers requested, but not available!");
	}
	#endif

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

	auto extensions = GetRequiredExtensions();
	CreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	CreateInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	#ifdef DEBUG
	    CreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	    CreateInfo.ppEnabledLayerNames = validationLayers.data();

		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
	    debugCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	#else
		CreateInfo.enabledLayerCount = 0;

		CreateInfo.pNext = nullptr;
	#endif

	if (vkCreateInstance(&CreateInfo, nullptr, &(instance)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to Create instance!");
	}

	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to Create window surface!");
	}
}


void Engine::PickPhysicalDevice(int(*ScoreDevice)(VkPhysicalDevice, VkSurfaceKHR, const std::vector<const char *>))
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

	for (const auto device : devices)
	{
		int score = ScoreDevice(device, surface, deviceExtensions);

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

void Engine::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

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

	#ifdef DEBUG
		CreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		CreateInfo.ppEnabledLayerNames = validationLayers.data();
	#else
		CreateInfo.enabledLayerCount = 0;
	#endif

	if (vkCreateDevice(physicalDevice, &CreateInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to Create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void Engine::SetupDebugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT debugCallback)
{
	VkDebugUtilsMessengerCreateInfoEXT CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	CreateInfo.pfnUserCallback = debugCallback;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, &CreateInfo, nullptr, &debugMessenger);
	}
	else
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}