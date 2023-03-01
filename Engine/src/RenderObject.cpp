#include "../include/RenderObject.hpp"

RenderObject::RenderObject(Mesh mesh, Transform transform, Pipeline pipeline) : mesh(mesh), transform(transform), pipeline(pipeline)
{
    for(unsigned int i = 0; i < Engine::engine->MaxFramesInFlight; i++)
    {
        Buffer ubo(sizeof(Transform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        ubo.CopyFromCPU(&transform, sizeof(Transform));

        descriptorSets.push_back(DescriptorSet({Descriptor(ubo)}));

        descriptorSets[i].set = (pipeline.setMakers[1].CreateDescriptorSet(descriptorSets[i].descriptors));
    }
}

RenderObject RenderObject::CreateSquare(Transform transform, Pipeline pipeline, double r, double g, double b)
{
    return RenderObject(Mesh::CreateSquare(r, g, b), transform, pipeline);
}

RenderObject RenderObject::CreateCube(Transform transform, Pipeline pipeline, double r, double g, double b)
{
    return RenderObject(Mesh::CreateCube(r, g, b), transform, pipeline);
}

RenderObject RenderObject::CreateCircle(Transform transform, Pipeline pipeline, int detail, double r, double g, double b)
{
	return RenderObject(Mesh::CreateCircle(detail, r, g, b), transform, pipeline);
}