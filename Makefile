CFLAGS=-std=c++17 -g -O0 -DDEBUG -pedantic -Wall -Wextra -I/home/qwob/Projects/Wheel/Vulkan/stb
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lpng

Main: *.cpp
	g++ $(CFLAGS) -o main main.cpp $(LDFLAGS)

.PHONY: test clean

test: Vulkan
	./Vulkan

clean:
	rm -f Vulkan
	rm core*
