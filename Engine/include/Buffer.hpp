#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Image.hpp"

class Buffer
{
public:
    VkBuffer buffer;
    VkDeviceMemory memory;
	size_t size;

    Buffer();
    Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

    static Buffer CreateVertexBuffer(void* vertexData, VkDeviceSize bufferSize);
    static Buffer CreateIndexBuffer(void* indexData, VkDeviceSize bufferSize);

	void CopyFromCPU(void* data, size_t size);
	size_t CopyToCPU(void* dst);
    void CopyToBuffer(Buffer buffer);
    void CopyToImage(Image dst);

    void Destroy();
};
