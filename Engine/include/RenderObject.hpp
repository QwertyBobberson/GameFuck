#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "HelperStructs.hpp"
#include "Buffer.hpp"
#include "Mesh.hpp"
#include "Engine.hpp"
#include "Pipeline.hpp"
#include "DescriptorSet.hpp"
#include <vector>

class RenderObject
{
public:
    Mesh mesh;
    Transform transform;
    Pipeline pipeline;
    std::vector<DescriptorSet> descriptorSets;
    RenderObject(Mesh mesh, Transform _transform, Pipeline pipeline);

    static RenderObject CreateSquare(Transform transform, Pipeline pipeline, double r, double g, double b);
    static RenderObject CreateCube(Transform transform, Pipeline pipeline, double r = -1.0f, double g = -1.0f, double b = -1.0f);
    static RenderObject CreateCircle(Transform transform, Pipeline pipeline, int detail, double r, double g, double b);
};