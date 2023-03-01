#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>

#include <optional>
#include <vector>
#include <stdio.h>

struct QueueFamilyIndices
{
	std::optional<unsigned int> graphicsFamily;
	std::optional<unsigned int> presentFamily;

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

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Attachment
{
	VkAttachmentDescription description;
	VkAttachmentReference2 refrence;
};

struct Transform
{
	glm::mat4 pos;
	glm::mat4 rotation;
	glm::mat4 scale;
};

struct ViewProjection
{
	glm::mat4 view;
	glm::mat4 projection;
};