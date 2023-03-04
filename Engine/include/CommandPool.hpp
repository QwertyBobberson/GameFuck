#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

struct CommandPool
{
    VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

    CommandPool();

	void CreateCommandPool();

	void CreateCommandBuffers();

	VkCommandBuffer BeginSingleTimeCommands();

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
};
