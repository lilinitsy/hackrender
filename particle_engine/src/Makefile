CXXFLAGS := -I/usr/include/SDL2/ -I/usr/include/assimp/ -std=c++11
LDFLAGS := -g -flto -lSDL2 -lSDL2main -lSDL2_mixer -lGL -ldl -lassimp -lX11 

CPP_OBJS := glad/glad.o main.o Shader.o
all: example
clean:
	rm -f example $(CPP_OBJS)

example: $(CPP_OBJS)
	clang++ $(LDFLAGS) -o $@ $^

%.o: %.cpp
	clang++ $(CXXFLAGS) -c -o $@ $<
