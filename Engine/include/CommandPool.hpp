#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>

struct Engine;

struct CommandPool
{
    Engine* engine;
    VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

    CommandPool(Engine* _engine);

	void CreateCommandPool();

	void CreateCommandBuffers();

	VkCommandBuffer BeginSingleTimeCommands();

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
};
#include "Engine.hpp"