#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <cstdint>
#include <vector>
#include "CommandPool.hpp"

struct SwapChain;

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Image
{
	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec3 texCoord;
};

struct Attachment
{
	VkAttachmentDescription description;
	VkAttachmentReference2 refrence;
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME});

int isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const std::vector<const char *> deviceExtensions);

std::vector<const char *> GetRequiredExtensions();

void CreateImage(Engine* engine, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

VkImageView CreateImageView(Engine* engine, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

uint32_t FindMemoryType(Engine* engine, uint32_t typeFilter, VkMemoryPropertyFlags properties);

void CreateBuffer(Engine* engine, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

void CopyBuffer(CommandPool cmdPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

void CreateVertexBuffer(CommandPool cmdPool, VkDeviceSize bufferSize, void* data, VkBuffer& buffer, VkDeviceMemory& memory);

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

bool CheckValidationLayerSupport(Engine* engine);

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

bool hasStencilComponent(VkFormat format);

Image CreateDepthImage(CommandPool cmdPool, SwapChain swapChain);

void CreateIndexBuffer(CommandPool cmdPool, VkDeviceSize bufferSize, void* vertexData, VkBuffer& buffer, VkDeviceMemory& memory);

#include "SwapChain.hpp"