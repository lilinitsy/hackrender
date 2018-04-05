CXX := clang++
LDFLAGS := -g -fopenmp -flto -lSDL2 -lSDL2main -lSDL2_mixer -lGL -ldl -lX11 -lassimp -I/usr/include/SDL2/ -I/usr/include/assimp/ -std=c++11

CPP_OBJS := glad/glad.o main.o Shader.o
all: example
clean:
	rm -f example $(CPP_OBJS)

example: $(CPP_OBJS)
	$(CXX) $(LDFLAGS) -o $(@) $(^)

%.o: %.cpp
	$(CXX) $(LDFLAGS) -c -o $(@) $(<)
