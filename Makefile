CPPFLAGS= -std=c++17 -g -O0 -DDEBUG -pedantic -Wall -Wextra
LDFLAGS = -I Engine/include/ -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lpng

VPATH = Engine/src:Engine/lib:Engine/include

all : main shaders/frag.spv shaders/vert.spv

main : main.cpp Engine.o SwapChain.o Camera.o Input.o
	g++ $(CPPFLAGS) -o main main.cpp Engine/lib/* $(LDFLAGS)

Buffer.o : Buffer.cpp Buffer.hpp Image.o Engine.o CommandPool.o Device.o
	g++ $(CPPFLAGS) -o Engine/lib/Buffer.o -c Engine/src/Buffer.cpp $(LDFLAGS)

Camera.o : Camera.cpp Camera.hpp HelperStructs.o DescriptorSet.o Pipeline.o RenderObject.o SwapChain.o RenderPass.o
	g++ $(CPPFLAGS) -o Engine/lib/Camera.o -c Engine/src/Camera.cpp $(LDFLAGS)

CommandPool.o : CommandPool.cpp CommandPool.hpp Device.o Engine.o
	g++ $(CPPFLAGS) -o Engine/lib/CommandPool.o -c Engine/src/CommandPool.cpp $(LDFLAGS)

Descriptor.o : Descriptor.cpp Descriptor.hpp Buffer.o Image.o
	g++ $(CPPFLAGS) -o Engine/lib/Descriptor.o -c Engine/src/Descriptor.cpp $(LDFLAGS)

DescriptorSet.o : DescriptorSet.cpp DescriptorSet.hpp Descriptor.o
	g++ $(CPPFLAGS) -o Engine/lib/DescriptorSet.o -c Engine/src/DescriptorSet.cpp $(LDFLAGS)

DescriptorSetMaker.o : DescriptorSetMaker.cpp DescriptorSetMaker.hpp DescriptorSet.o
	g++ $(CPPFLAGS) -o Engine/lib/DescriptorSetMaker.o -c Engine/src/DescriptorSetMaker.cpp $(LDFLAGS)

Device.o : Device.cpp Device.hpp HelperStructs.o
	g++ $(CPPFLAGS) -o Engine/lib/Device.o -c Engine/src/Device.cpp $(LDFLAGS)

Engine.o : Engine.cpp Engine.hpp Device.o HelperStructs.o
	g++ $(CPPFLAGS) -o Engine/lib/Engine.o -c Engine/src/Engine.cpp $(LDFLAGS)

FrameBuffer.o : FrameBuffer.cpp FrameBuffer.hpp Image.o RenderPass.o
	g++ $(CPPFLAGS) -o Engine/lib/FrameBuffer.o -c Engine/src/FrameBuffer.cpp $(LDFLAGS)

HelperStructs.o : HelperStructs.cpp HelperStructs.hpp
	g++ $(CPPFLAGS) -o Engine/lib/HelperStructs.o -c Engine/src/HelperStructs.cpp $(LDFLAGS)

Image.o : Image.cpp Image.hpp Engine.o
	g++ $(CPPFLAGS) -o Engine/lib/Image.o -c Engine/src/Image.cpp $(LDFLAGS)

Input.o : Input.cpp Input.hpp
	g++ $(CPPFLAGS) -o Engine/lib/Input.o -c Engine/src/Input.cpp $(LDFLAGS)

Material.o : Material.cpp Material.hpp Pipeline.o
	g++ $(CPPFLAGS) -o Engine/lib/Material.o -c Engine/src/Material.cpp $(LDFLAGS)

Mesh.o : Mesh.cpp Mesh.hpp Buffer.o
	g++ $(CPPFLAGS) -o Engine/lib/Mesh.o -c Engine/src/Mesh.cpp $(LDFLAGS)

Pipeline.o : Pipeline.cpp Pipeline.hpp DescriptorSetMaker.o SwapChain.o Engine.o RenderPass.o
	g++ $(CPPFLAGS) -o Engine/lib/Pipeline.o -c Engine/src/Pipeline.cpp $(LDFLAGS)

RenderObject.o : RenderObject.cpp RenderObject.hpp DescriptorSet.o Pipeline.o Engine.o Mesh.o Buffer.o HelperStructs.o
	g++ $(CPPFLAGS) -o Engine/lib/RenderObject.o -c Engine/src/RenderObject.cpp $(LDFLAGS)

SwapChain.o : SwapChain.cpp SwapChain.hpp FrameBuffer.o Image.o Engine.o RenderPass.o
	g++ $(CPPFLAGS) -o Engine/lib/SwapChain.o -c Engine/src/SwapChain.cpp $(LDFLAGS)

RenderPass.o : RenderPass.cpp RenderPass.hpp Engine.o Device.o
	g++ $(CPPFLAGS) -o Engine/lib/RenderPass.o -c Engine/src/RenderPass.cpp $(LDFLAGS)

shaders/frag.spv : shaders/shader.frag
	glslc -c shaders/shader.frag -o shaders/frag.spv

shaders/vert.spv : shaders/shader.vert
	glslc -c shaders/shader.vert -o shaders/vert.spv

.PHONY: test clean

clean:
	rm Engine/lib/*.o
	rm main