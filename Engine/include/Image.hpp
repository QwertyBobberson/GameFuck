#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class Image
{
public:
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
	VkFormat format;
	VkImageLayout layout;
    VkExtent2D extent;

    Image(VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL, VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,  VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT, VkExtent2D extent = {1, 1});
    Image(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkExtent2D extent);

    static Image CreateDepthImage(VkExtent2D);
    static std::vector<Image> CreateImagesFromSwapchain(VkSwapchainKHR swapchin, VkFormat format, VkExtent2D extent);

	void TransitionLayout(VkImageLayout newLayout);
    VkSampler CreateSampler();
    void CopyToImage(Image dst);
    void CopyToCPU(char* dst);
    void SaveToBMP(char* file);

    void Destroy();
private:
    static VkImageView CreateView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
};
