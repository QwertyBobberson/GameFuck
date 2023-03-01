#include "../include/Mesh.hpp"

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<int> indices)
{
    indexCount = indices.size();
    vertexBuffer = Buffer::CreateVertexBuffer(vertices.data(), sizeof(Vertex) * vertices.size());
    indexBuffer = Buffer::CreateIndexBuffer(indices.data(), sizeof(int) * indexCount);
}

void Mesh::Destroy()
{
    vertexBuffer.Destroy();
    indexBuffer.Destroy();
}

Mesh Mesh::CreateSquare(double r, double g, double b)
{
    float x = -(1/2.0f);
    float y = -(1/2.0f);
    Mesh mesh(
        {
            {{x		, y		, 0.0f}, {r, g, b}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{x + 1	, y	    , 0.0f}, {r, g, b}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{x		, y + 1 , 0.0f}, {r, g, b}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{x + 1	, y + 1 , 0.0f}, {r, g, b}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}
        },
        {0, 1, 2, 2, 3, 1});
    return mesh;
}

Mesh Mesh::CreateCube(double r, double g, double b)
{
    glm::vec3 corner(-1/2.0f, -1/2.0f, -1/2.0f);

    std::vector<Vertex> vertices;

    if(r != -1.0f)
    {
        for(int i = 0; i < 8; i++)
        {
            vertices.push_back({{corner + glm::vec3(i & 1, i >> 1 & 1, i >> 2 & 1)}, {r, g, b}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
        }
    }
    else
    {
        for(int i = 0; i < 8; i++)
        {
            vertices.push_back({{corner + glm::vec3(i & 1, i >> 1 & 1, i >> 2 & 1)}, {i & 1, i >> 1 & 1, i >> 2 & 1}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
        }
    }

    std::vector<int> indices =
    {
         4,  5,  6,  6,  5,  7,
         5,  1,  7,  7,  1,  3,
         1,  0,  3,  3,  0,  2,
         0,  4,  2,  2,  4,  6,
         1,  5,  0,  0,  5,  4,
         2,  6,  3,  3,  6,  7
    };

    int vertOccurenceCount[8] = {0};

    for(int i = 0; i < indices.size(); i += 3)
    {
        glm::vec3 a = vertices[indices[i]].pos;
        glm::vec3 b = vertices[indices[i + 1]].pos;
        glm::vec3 c = vertices[indices[i + 2]].pos;

        glm::vec3 cross = glm::normalize(glm::cross(b - a, c - a));

        vertices[indices[i]].normal += cross;
        vertices[indices[i + 1]].normal += cross;
        vertices[indices[i + 2]].normal += cross;

        vertOccurenceCount[indices[i]]++;
        vertOccurenceCount[indices[i + 1]]++;
        vertOccurenceCount[indices[i + 2]]++;
    }

    for(int i = 0; i < vertices.size(); i++)
    {
        vertices[i].normal = glm::normalize(vertices[i].normal);
        printf("%d Normal: (%f, %f, %f) (Appeared %d times)\n", i, vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z, vertOccurenceCount[i]);
    }

    return Mesh(vertices, indices);
}

Mesh Mesh::CreateCircle(int detail, double r, double g, double b)
{
	std::vector<Vertex> verts;
	std::vector<int> indices;

	glm::vec2 pos(0, 0);
	verts.push_back({{pos.x, pos.y, 0.0f}, {r, g, b}, {0.0f, 0.0f}});
	for(int i = 0; i < detail; i++)
	{
		glm::vec2 dir(cos(-(M_PI/2) + M_PI * 2 * ((float)i/detail)), sin(-(M_PI/2) + M_PI * 2 * ((float)i/detail)));
		glm::vec2 edgePos = pos + dir;
		verts.push_back({{edgePos.x, edgePos.y, 0.0f}, {r, g, b}, {0.0f, 0.0f}});
	}

	for(int i = 0; i < detail - 1; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
	}
	indices.push_back(0);
	indices.push_back(detail);
	indices.push_back(1);
	Mesh mesh(verts, indices);
	return mesh;
}