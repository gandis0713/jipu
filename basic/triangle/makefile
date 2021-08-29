CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

all: clean build

build: clean
	g++ $(CFLAGS) -o triangle.out main.cpp $(LDFLAGS)

.PHONY: test clean

test: triangle.out
	./triangle.out

clean:
	rm -f triangle.out