#include "Application.cpp"
using namespace std;

int main(int argc, char** argv)
{
	Application app;

	std::vector<Vertex> verts;// = 
	// {
	// 	{{-1.0f, -0.5f}, {0.0f, 0.0f, 1.0f}},
	// 	{{-0.5f, -1.0f}, {0.0f, 0.0f, 1.0f}},
	// 	{{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},

	// 	{{-1.0f, -0.5f}, {0.0f, 0.0f, 1.0f}},
	// 	{{-1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
	// 	{{-0.5f, -1.0f}, {0.0f, 0.0f, 1.0f}}
	// };
	
	double size = std::stod(argv[1]);
	for(int y = -size; y < size; y += 1)
	{
		for(int x = -size; x < size; x += 1)
		{
			if((x + y) % 2 == 0)
			{
				verts.push_back({{x/size, 		 y/size + size}, {0, 0.05, .95}});
				verts.push_back({{x/size + size, y/size}, 		 {0, 0.05, .95}});
				verts.push_back({{x/size + size, y/size + size}, {0, 0.05, .95}});
				verts.push_back({{x/size, 		 y/size + size}, {0, 0.05, .95}});
				verts.push_back({{x/size, 		 y/size}, 		 {0, 0.05, .95}});
				verts.push_back({{x/size + size, y/size}, 		 {0, 0.05, .95}});
			}
			else
			{
				verts.push_back({{x/size, 		 y/size + size}, {0, 0, 0}});
				verts.push_back({{x/size + size, y/size}, 		 {0, 0, 0}});
				verts.push_back({{x/size + size, y/size + size}, {0, 0, 0}});
				verts.push_back({{x/size, 		 y/size + size}, {0, 0, 0}});
				verts.push_back({{x/size, 		 y/size}, 		 {0, 0, 0}});
				verts.push_back({{x/size + size, y/size}, 		 {0, 0, 0}});
			}
		}
	}

	try
	{
		app.Run(verts);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
	
}