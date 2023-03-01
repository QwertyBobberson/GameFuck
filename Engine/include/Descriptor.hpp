#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Buffer.hpp"
#include "Image.hpp"

union Data
{
    Buffer buffer;
    Image image;

    Data();
};

enum DescriptorType {buffer, image};

class Descriptor
{
public:

    Data data;
    DescriptorType type;
    enum {BUFFER_DESCRIPTOR, IMAGE_DESCRIPTOR};
    void* info;

    Descriptor(Buffer buffer);
    Descriptor(Image image);
};
