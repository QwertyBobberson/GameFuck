#pragma once

#include "Buffer.hpp"
#include "HelperStructs.hpp"
#include <vector>

class Mesh
{
public:
    Buffer vertexBuffer;
    Buffer indexBuffer;
    int indexCount;

    Mesh();
    Mesh(std::vector<Vertex> vertices, std::vector<int> indices);

    static Mesh CreateSquare(double r, double g, double b);
    static Mesh CreateCube(double r = -1.0f, double g = -1.0f, double b = -1.0f);
    static Mesh CreateCircle(int detail, double r, double g, double b);

    void Destroy();
};
