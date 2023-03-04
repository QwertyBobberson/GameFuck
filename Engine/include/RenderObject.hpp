#pragma once
#include "HelperStructs.hpp"
#include "Mesh.hpp"
#include "Pipeline.hpp"
#include "DescriptorSet.hpp"
#include <vector>

class RenderObject
{
public:
    Mesh mesh;
    Image texture;
    Transform transform;
    Pipeline pipeline;
    std::vector<DescriptorSet> descriptorSets;
    RenderObject(Mesh mesh, Image texture, Transform transform, Pipeline pipeline);

    static RenderObject CreateSquare(Transform transform, Image texture, Pipeline pipeline, double r, double g, double b);
    static RenderObject CreateCube(Transform transform, Image texture, Pipeline pipeline, double r = -1.0f, double g = -1.0f, double b = -1.0f);
    static RenderObject CreateCircle(Transform transform, Image texture, Pipeline pipeline, int detail, double r, double g, double b);
};