#include "../include/Engine.hpp"

Engine* Engine::engine = nullptr;

Engine::Engine(const char* applicationName, unsigned int width, unsigned int height, GLFWframebuffersizefun framebufferResizeCallback, int(*ScoreDevice)(VkPhysicalDevice, VkSurfaceKHR, const std::vector<const char *>), int framesInFlight) : MaxFramesInFlight(framesInFlight), extent{width, height}
{
	if(engine != nullptr)
	{
		printf("There is more than one engine object\n");
	}
	engine = this;
	framebufferResizeCallback = framebufferResizeCallback == nullptr ? FramebufferResizeCallback : framebufferResizeCallback;
	ScoreDevice = ScoreDevice == nullptr ? Device::isDeviceSuitable : ScoreDevice;
	SetupWindow(applicationName, framebufferResizeCallback, DebugCallback);
	PickPhysicalDevice(ScoreDevice);
	CreateLogicalDevice();
	#ifdef DEBUG
		SetupDebugMessenger(DebugCallback);
	#endif

	swapChainSupport = Device::QuerySwapChainSupport(Device::physicalDevice, surface);

	surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	extent = ChooseSwapExtent(swapChainSupport.capabilities);

	cmdPool = new CommandPool();

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MousePosCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Engine::SetupWindow(const char* applicationName, GLFWframebuffersizefun framebufferResizeCallback, PFN_vkDebugUtilsMessengerCallbackEXT debugCallback)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(extent.width, extent.height, applicationName, nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	#ifdef DEBUG
	if (!CheckValidationLayerSupport())
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
	CreateInfo.enabledExtensionCount = static_cast<unsigned int>(extensions.size());
	CreateInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	#ifdef DEBUG
	    CreateInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
	    CreateInfo.ppEnabledLayerNames = validationLayers.data();

		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
	    debugCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	#else
		CreateInfo.enabledLayerCount = 0;
		#warning "Not compiling with debug flag"
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
	unsigned int deviceCount = 0;
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
	Device::physicalDevice = physicalDevice;
}

void Engine::CreateLogicalDevice()
{
	QueueFamilyIndices indices = Device::FindQueueFamilies(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<unsigned int> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (unsigned int queueFamily : uniqueQueueFamilies)
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

	CreateInfo.queueCreateInfoCount = static_cast<unsigned int>(queueCreateInfos.size());
	CreateInfo.pQueueCreateInfos = queueCreateInfos.data();

	CreateInfo.pEnabledFeatures = &deviceFeatures;

	CreateInfo.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size());
	CreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	#ifdef DEBUG
		CreateInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
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
	CreateInfo.pUserData = nullptr;

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

VkExtent2D Engine::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<unsigned int>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<unsigned int>(width),
            static_cast<unsigned int>(height)};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkPresentModeKHR Engine::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR Engine::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}


std::vector<const char *> Engine::GetRequiredExtensions()
{
    unsigned int glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    #ifdef DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

    return extensions;
}

bool Engine::CheckValidationLayerSupport()
{
    unsigned int layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

void Engine::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    while(width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(Engine::engine->device);
    Engine::engine->extent = {(unsigned int)width, (unsigned int)height};
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    std::cout << messageSeverity << ": " << messageType << " validation layer: " << pCallbackData->pMessage << "\n\n\n";

    if(pUserData != nullptr)
    {
        std::cout << "User Data: " << pUserData << "\n";
    }

    return VK_FALSE;
}