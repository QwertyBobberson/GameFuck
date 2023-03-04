#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../include/RenderObject.hpp"
#include "../include/Engine.hpp"

RenderObject::RenderObject(Mesh mesh, Image texture, Transform transform, Pipeline pipeline) : mesh(mesh), texture(texture), transform(transform), pipeline(pipeline)
{
    for(unsigned int i = 0; i < Engine::engine->MaxFramesInFlight; i++)
    {
        Buffer ubo(sizeof(Transform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        ubo.CopyFromCPU(&transform, sizeof(Transform));

        descriptorSets.push_back(DescriptorSet({Descriptor(ubo), Descriptor(texture)}));

        descriptorSets[i].set = (pipeline.setMakers[1].CreateDescriptorSet(descriptorSets[i].descriptors));
    }
}

RenderObject RenderObject::CreateSquare(Transform transform, Image texture, Pipeline pipeline, double r, double g, double b)
{
    return RenderObject(Mesh::CreateSquare(r, g, b), texture, transform, pipeline);
}

RenderObject RenderObject::CreateCube(Transform transform, Image texture, Pipeline pipeline, double r, double g, double b)
{
    return RenderObject(Mesh::CreateCube(r, g, b), texture, transform, pipeline);
}

RenderObject RenderObject::CreateCircle(Transform transform, Image texture, Pipeline pipeline, int detail, double r, double g, double b)
{
	return RenderObject(Mesh::CreateCircle(detail, r, g, b), texture, transform, pipeline);
}