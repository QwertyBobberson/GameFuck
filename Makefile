CFLAGS= -std=c++17 -g -O0 -DDEBUG -pedantic -Wall -Wextra
LDFLAGS = -I Engine/include/ -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lpng

# Main: *.cpp
# 	g++ $(CFLAGS) -o main Engine/src/* main.cpp $(LDFLAGS)

all : main shaders/frag.spv shaders/vert.spv

main : main.cpp Engine/lib/Engine.o Engine/lib/SwapChain.o Engine/lib/Camera.o Engine/lib/Input.o
	g++ $(CFLAGS) -o main main.cpp Engine/lib/Buffer.o Engine/lib/Camera.o Engine/lib/CommandPool.o Engine/lib/Descriptor.o Engine/lib/DescriptorSet.o Engine/lib/DescriptorSetMaker.o Engine/lib/Device.o Engine/lib/Engine.o Engine/lib/FrameBuffer.o Engine/lib/HelperStructs.o Engine/lib/Image.o Engine/lib/Input.o Engine/lib/Material.o Engine/lib/Mesh.o Engine/lib/Pipeline.o Engine/lib/RenderObject.o Engine/lib/SwapChain.o $(LDFLAGS)

Engine/lib/Buffer.o : Engine/src/Buffer.cpp Engine/include/Buffer.hpp Engine/lib/Image.o Engine/lib/Engine.o Engine/lib/CommandPool.o Engine/lib/Device.o
	g++ $(CFLAGS) -o Engine/lib/Buffer.o -c Engine/src/Buffer.cpp $(LDFLAGS)

Engine/lib/Camera.o : Engine/src/Camera.cpp Engine/include/Camera.hpp Engine/lib/HelperStructs.o Engine/lib/DescriptorSet.o Engine/lib/Pipeline.o Engine/lib/RenderObject.o Engine/lib/SwapChain.o
	g++ $(CFLAGS) -o Engine/lib/Camera.o -c Engine/src/Camera.cpp $(LDFLAGS)

Engine/lib/CommandPool.o : Engine/src/CommandPool.cpp Engine/include/CommandPool.hpp Engine/lib/Device.o Engine/lib/Engine.o
	g++ $(CFLAGS) -o Engine/lib/CommandPool.o -c Engine/src/CommandPool.cpp $(LDFLAGS)

Engine/lib/Descriptor.o : Engine/src/Descriptor.cpp Engine/include/Descriptor.hpp Engine/lib/Buffer.o Engine/lib/Image.o
	g++ $(CFLAGS) -o Engine/lib/Descriptor.o -c Engine/src/Descriptor.cpp $(LDFLAGS)

Engine/lib/DescriptorSet.o : Engine/src/DescriptorSet.cpp Engine/include/DescriptorSet.hpp Engine/lib/Descriptor.o
	g++ $(CFLAGS) -o Engine/lib/DescriptorSet.o -c Engine/src/DescriptorSet.cpp $(LDFLAGS)

Engine/lib/DescriptorSetMaker.o : Engine/src/DescriptorSetMaker.cpp Engine/include/DescriptorSetMaker.hpp Engine/lib/DescriptorSet.o
	g++ $(CFLAGS) -o Engine/lib/DescriptorSetMaker.o -c Engine/src/DescriptorSetMaker.cpp $(LDFLAGS)

Engine/lib/Device.o : Engine/src/Device.cpp Engine/include/Device.hpp Engine/lib/HelperStructs.o
	g++ $(CFLAGS) -o Engine/lib/Device.o -c Engine/src/Device.cpp $(LDFLAGS)

Engine/lib/Engine.o : Engine/src/Engine.cpp Engine/include/Engine.hpp Engine/lib/Device.o Engine/lib/HelperStructs.o
	g++ $(CFLAGS) -o Engine/lib/Engine.o -c Engine/src/Engine.cpp $(LDFLAGS)

Engine/lib/FrameBuffer.o : Engine/src/FrameBuffer.cpp Engine/include/FrameBuffer.hpp Engine/lib/Image.o
	g++ $(CFLAGS) -o Engine/lib/FrameBuffer.o -c Engine/src/FrameBuffer.cpp $(LDFLAGS)

Engine/lib/HelperStructs.o : Engine/src/HelperStructs.cpp Engine/include/HelperStructs.hpp
	g++ $(CFLAGS) -o Engine/lib/HelperStructs.o -c Engine/src/HelperStructs.cpp $(LDFLAGS)

Engine/lib/Image.o : Engine/src/Image.cpp Engine/include/Image.hpp Engine/lib/Engine.o
	g++ $(CFLAGS) -o Engine/lib/Image.o -c Engine/src/Image.cpp $(LDFLAGS)

Engine/lib/Input.o : Engine/src/Input.cpp Engine/include/Input.hpp
	g++ $(CFLAGS) -o Engine/lib/Input.o -c Engine/src/Input.cpp $(LDFLAGS)

Engine/lib/Material.o : Engine/src/Material.cpp Engine/include/Material.hpp Engine/lib/Pipeline.o
	g++ $(CFLAGS) -o Engine/lib/Material.o -c Engine/src/Material.cpp $(LDFLAGS)

Engine/lib/Mesh.o : Engine/src/Mesh.cpp Engine/include/Mesh.hpp Engine/lib/Buffer.o
	g++ $(CFLAGS) -o Engine/lib/Mesh.o -c Engine/src/Mesh.cpp $(LDFLAGS)

Engine/lib/Pipeline.o : Engine/src/Pipeline.cpp Engine/include/Pipeline.hpp Engine/lib/DescriptorSetMaker.o Engine/lib/SwapChain.o Engine/lib/Engine.o
	g++ $(CFLAGS) -o Engine/lib/Pipeline.o -c Engine/src/Pipeline.cpp $(LDFLAGS)

Engine/lib/RenderObject.o : Engine/src/RenderObject.cpp Engine/include/RenderObject.hpp Engine/lib/DescriptorSet.o  Engine/lib/Pipeline.o  Engine/lib/Engine.o  Engine/lib/Mesh.o  Engine/lib/Buffer.o  Engine/lib/HelperStructs.o
	g++ $(CFLAGS) -o Engine/lib/RenderObject.o -c Engine/src/RenderObject.cpp $(LDFLAGS)

Engine/lib/SwapChain.o : Engine/src/SwapChain.cpp Engine/include/SwapChain.hpp Engine/lib/FrameBuffer.o Engine/lib/Image.o Engine/lib/Engine.o
	g++ $(CFLAGS) -o Engine/lib/SwapChain.o -c Engine/src/SwapChain.cpp $(LDFLAGS)

.PHONY: test clean shaders

shaders:
	glslc -c shaders/shader.frag -o shaders/frag.spv
	glslc -c shaders/shader.vert -o shaders/vert.spv

shaders/frag.spv : shaders/shader.frag
	glslc -c shaders/shader.frag -o shaders/frag.spv

shaders/vert.spv : shaders/shader.vert
	glslc -c shaders/shader.vert -o shaders/vert.spv


test: Vulkan
	./Vulkan

clean:
	rm -f Vulkan
	rm core*
