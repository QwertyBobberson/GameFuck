#include "Application.cpp"
using namespace std;

void DrawSquare(double x, double y, double width, double height, double r, double g, double b)
{
	int start = verts.size();
	verts.push_back({{x			, y + height}, {r, g, b}});
	verts.push_back({{x + width	, y			}, {r, g, b}});
	verts.push_back({{x + width	, y + height}, {r, g, b}});
	verts.push_back({{x			, y			}, {r, g, b}});

	indices.push_back(start + 0);
	indices.push_back(start + 1);
	indices.push_back(start + 2);
	indices.push_back(start + 0);
	indices.push_back(start + 3);
	indices.push_back(start + 1);
}


int main(int argc, char** argv)
{
	Application app;

	// verts = 
	// {
	// 	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	// 	{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	// 	{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
	// 	{{-0.5f,  0.5f}, {0.0f, 0.0f, 0.0f}}
	// };

	// indices = 
	// {
	// 	0, 1, 2, 
	// 	2, 3, 0
	// };

	double size = std::stod(argv[1]);
	for(int y = -size; y < size; y += 1)
	{
		for(int x = -size; x < size; x += 1)
		{
			if((x + y) % 2 == 0)
			{
				DrawSquare(x/size, y/size, 1/size, 1/size, 0, 0.05, 0.95);
			}
			else
			{
				DrawSquare(x/size, y/size, 1/size, 1/size, 0, 0, 0);
			}
		}
	}

	try
	{
		app.Run(verts, indices);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
	
}