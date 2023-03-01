#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "Engine.hpp"

class Image
{
public:
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
	VkFormat format;
	VkImageLayout layout;
    VkExtent2D extent;

    Image();
    Image(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,  VkImageAspectFlags aspectFlags, VkExtent2D extent);
    Image(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkExtent2D extent);

    static Image CreateDepthImage(VkExtent2D);
    static std::vector<Image> CreateImagesFromSwapchain(VkSwapchainKHR swapchin, VkFormat format, VkExtent2D extent);

	void TransitionLayout(VkImageLayout newLayout);
    VkSampler CreateSampler();

    void Destroy();
private:
    static VkImageView CreateView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
};
