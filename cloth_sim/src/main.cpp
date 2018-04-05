#include "glad/glad.h"  //Include order can matter here

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Ball.h"
#include "Cloth.h"
#include "Mesh.h"
#include "Player.h"
#include "Shader.h"
#include "utils.h"

using namespace std;



Cloth set_default_cloth(int dimension);
Mesh set_default_ball();

int main(int argc, char *argv[])
{
    int screen_width = 1280;
    int screen_height = 1080;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_Window* window = SDL_CreateWindow("CSCI 5611 Homework 1", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    float yaw = -90.0f;
    float pitch = 0.0f;

    if (gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));
	}

	else
    {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}

    srand(time(NULL));

    Options options;
    Player player(glm::vec3(0.0f, 1.0f, -5.0f));
    Shader default_shader("shaders/vertexModel.glsl", "shaders/fragmentModel.glsl");
    default_shader.use();

    float planeVertices[] =
    {
        100.0f, -0.5f, 100.0f,  2.0f, 0.0f,
        0.0f, -0.5f, 100.0f,    0.0f, 0.0f,
        0.0f, -0.5f, 0.0f,      0.0f, 2.0f,

        100.0f, -0.5f, 100.0f,  2.0f, 0.0f,
        0.0f, -0.5f, 0.0f,      0.0f, 2.0f,
        100.0f, -0.5f, 0.0f,    2.0f, 2.0f
    };

    Mesh *plane = new Mesh(planeVertices, 30, false, true);   
    plane->model = glm::mat4();
    plane->in_colour = glm::vec4(0.2f, 0.8f, 0.5f, 0.9f);

	SDL_Event windowEvent;
	bool quit = false;

    float current_time = SDL_GetPerformanceCounter();	// time between current frame and last frame
    float last_time = 0;

    Cloth cloth = set_default_cloth(70);

    Ball ball("bally", 10.0f);
    ball.mesh = set_default_ball();
    ball.mesh.model = glm::mat4();
    ball.radius = 0.375f;
    ball.origin = glm::vec3(0.0f, 0.0f, 0.0f);
    ball.mesh.in_colour = glm::vec4(0.8f, 0.3f, 0.3f, 0.8f);


	while (!quit)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        current_time = SDL_GetTicks();
        float delta_time = 0.004f;// (float)(current_time - last_time) / 5000.0f;
        float dt = (float)(current_time - last_time) / 1000.0f;
        last_time = current_time;
        float move = delta_time * 50.0f;

        bool validEvent = false;
        printf("fps: %f\n", 1.0f / dt);

        while(SDL_PollEvent(&windowEvent))
        {
            if(windowEvent.type == SDL_QUIT)
            {
                quit = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_w)
            {
                player.camera_pos = glm::vec3(player.camera_pos.x + player.camera_front.x * move, player.camera_pos.y + player.camera_front.y * move, player.camera_pos.z + player.camera_front.z * move);
                validEvent = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_s)
            {
                player.camera_pos = glm::vec3(player.camera_pos.x - player.camera_front.x * move, player.camera_pos.y - player.camera_front.y * move, player.camera_pos.z - player.camera_front.z * move);
                validEvent = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_a)
            {
                player.camera_pos -= glm::normalize(glm::cross(player.camera_front, player.camera_up)) * move;
                validEvent = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_d)
            {
                player.camera_pos += glm::normalize(glm::cross(player.camera_front, player.camera_up)) * move;
                validEvent = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT)
            {
                for(int i = 0; i < ball.mesh.vertices.size(); i += 3)
                {
                    ball.mesh.vertices[i] += 0.01f;
                }

                ball.origin.x += 0.01f;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT)
            {
                for(int i = 0; i < ball.mesh.vertices.size(); i += 3)
                {
                    ball.mesh.vertices[i] -= 0.01f;
                }

                ball.origin.x -= 0.01f;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP)
            {
                for(int i = 2; i < ball.mesh.vertices.size(); i += 3)
                {
                    ball.mesh.vertices[i] += 0.01f;
                }

                ball.origin.z += 0.01f;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN)
            {
                for(int i = 2; i < ball.mesh.vertices.size(); i += 3)
                {
                    ball.mesh.vertices[i] -= 0.01f;
                }

                ball.origin.z -= 0.01f;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_SPACE)
            {
                for(int i = 1; i < ball.mesh.vertices.size(); i += 3)
                {
                    ball.mesh.vertices[i] += 0.01f;
                }

                ball.origin.y += 0.01f;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LSHIFT)
            {
                for(int i = 1; i < ball.mesh.vertices.size(); i += 3)
                {
                    ball.mesh.vertices[i] -= 0.01f;
                }

                ball.origin.z -= 0.01f;
            }

            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_i)
            {
                for(int i = 0; i < cloth.width; i++)
                {
                    cloth.point_mass[i][0]->position += 0.1f;
                }
            }

            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_k)
            {
                for(int i = 0; i < cloth.width; i++)
                {
                    cloth.point_mass[i][0]->position -= 0.1f;
                }
            }
            
            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_l)
            {
                for(int i = 0; i < cloth.height; i++)
                {
                    cloth.point_mass[0][i]->position += 0.1f;
                }
            }

            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_j)
            {
                for(int i = 0; i < cloth.height; i++)
                {
                    cloth.point_mass[0][i]->position -= 0.1f;
                }
            }
        }

        glm::mat4 view = glm::lookAt(player.camera_pos, player.camera_pos + player.camera_front, player.camera_up);
        glm::mat4 proj = glm::perspective(3.14f/4, screen_width / (float) screen_height, 0.01f, 100.0f); //FOV, aspect, near, far
        default_shader.setMat4("projection", proj);
        default_shader.setMat4("view", view);
        default_shader.use();

        ball.mesh.model = glm::mat4();
        cloth.update(delta_time, ball);
        cloth.draw(default_shader);
        ball.mesh.draw(default_shader);

        SDL_GL_SwapWindow(window);
	}

    SDL_GL_DeleteContext(context); 
	SDL_Quit();

	return 0;
}

Cloth set_default_cloth(int dimension)
{
    Cloth cloth(dimension, dimension);

    for(int i = 0; i < dimension; i++)
    {
        for(int j = 0; j < dimension; j++)
        {
            float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 0.05f);
            PointMass *pm = new PointMass(glm::vec3(1.0f + (float) i / dimension, 1.0f - (float) j / dimension, 1.0f + z));
            cloth.add_point_mass(pm);
        }
    }

    cloth.init();
    cloth.mesh.model = glm::mat4();
    cloth.mesh.in_colour = glm::vec4(0.5f, 0.2f, 0.8f, 0.9f);

    return cloth;
}


Mesh set_default_ball()
{
    float mesh_verts[] = 
    {
        -0.25f,-0.25f,-0.25f, // triangle 1 : begin
        -0.25f,-0.25f, 0.25f,
        -0.25f, 0.25f, 0.25f, // triangle 1 : end
        0.25f, 0.25f,-0.25f, // triangle 2 : begin
        -0.25f,-0.25f,-0.25f,
        -0.25f, 0.25f,-0.25f, // triangle 2 : end
        0.25f,-0.25f, 0.25f,
        -0.25f,-0.25f,-0.25f,
        0.25f,-0.25f,-0.25f,
        0.25f, 0.25f,-0.25f,
        0.25f,-0.25f,-0.25f,
        -0.25f,-0.25f,-0.25f,
        -0.25f,-0.25f,-0.25f,
        -0.25f, 0.25f, 0.25f,
        -0.25f, 0.25f,-0.25f,
        0.25f,-0.25f, 0.25f,
        -0.25f,-0.25f, 0.25f,
        -0.25f,-0.25f,-0.25f,
        -0.25f, 0.25f, 0.25f,
        -0.25f,-0.25f, 0.25f,
        0.25f,-0.25f, 0.25f,
        0.25f, 0.25f, 0.25f,
        0.25f,-0.25f,-0.25f,
        0.25f, 0.25f,-0.25f,
        0.25f,-0.25f,-0.25f,
        0.25f, 0.25f, 0.25f,
        0.25f,-0.25f, 0.25f,
        0.25f, 0.25f, 0.25f,
        0.25f, 0.25f,-0.25f,
        -0.25f, 0.25f,-0.25f,
        0.25f, 0.25f, 0.25f,
        -0.25f, 0.25f,-0.25f,
        -0.25f, 0.25f, 0.25f,
        0.25f, 0.25f, 0.25f,
        -0.25f, 0.25f, 0.25f,
        0.25f,-0.25f, 0.25f
    };

    Mesh ball_mesh(mesh_verts, sizeof(mesh_verts) / sizeof(float), false, false);
    return ball_mesh;
}